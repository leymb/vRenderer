#include "pch.h"
#include "vRenderer/Image.h"
#include "vulkan/vulkan_core.h"
#include "vRenderer/Device.h"
#include "vRenderer/Buffer/Buffer.h"
#include <stdexcept>

#include "vRenderer/helpers/VulkanHelpers.h"

Image::Image()
= default;

Image::~Image()
= default;

void Image::CreateImage(const Device& a_Device, uint32_t a_Width, uint32_t a_Height, VkFormat a_Format, VkImageTiling a_Tiling,
                        VkImageUsageFlags a_UsageFlags, VkMemoryPropertyFlags a_PropertyFlags, VkImageAspectFlags a_ImageAspectFlag)
{
	VkImageCreateInfo t_ImageCreateInfo = {};
	t_ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	t_ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	t_ImageCreateInfo.extent.width = a_Width;
	t_ImageCreateInfo.extent.height = a_Height;
	t_ImageCreateInfo.extent.depth = 1;
	t_ImageCreateInfo.mipLevels = 1;
	t_ImageCreateInfo.arrayLayers = 1;

	// format needs to be the same for the texels as the pixels in the buffer
	t_ImageCreateInfo.format = a_Format;

	// decides how texels are laid out, in this case in implementation-defined order
	t_ImageCreateInfo.tiling = a_Tiling;

	// first transition will discard texels
	t_ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	t_ImageCreateInfo.usage = a_UsageFlags;
	t_ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	t_ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	t_ImageCreateInfo.flags = 0;

	if(vkCreateImage(a_Device.GetLogicalDevice(), &t_ImageCreateInfo, nullptr, &m_Image) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Could not create Image for Texture!");
	}

	AllocateImageMemory(a_Device, a_PropertyFlags);

	vkBindImageMemory(a_Device.GetLogicalDevice(), m_Image, m_ImageMemory, 0);

	m_ImageView = CreateImageView(a_Format, a_Device.GetLogicalDevice(),a_ImageAspectFlag);
}

void Image::DestroyImage(const VkDevice a_LogicalDevice)
{
	vkDestroyImageView(a_LogicalDevice, m_ImageView, nullptr);
	vkDestroyImage(a_LogicalDevice, m_Image, nullptr);
	vkFreeMemory(a_LogicalDevice, m_ImageMemory, nullptr);
}

VkImageView Image::CreateImageView(const VkFormat a_Format, const VkDevice a_LogicalDevice, VkImageAspectFlags a_AspectFlag) const
{
	VkImageViewCreateInfo t_ViewCreateInfo = {};
	t_ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	t_ViewCreateInfo.image = m_Image;
	t_ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	t_ViewCreateInfo.format = a_Format;

	t_ViewCreateInfo.subresourceRange.aspectMask = a_AspectFlag;
	t_ViewCreateInfo.subresourceRange.baseMipLevel = 0;
	t_ViewCreateInfo.subresourceRange.levelCount = 1;
	t_ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	t_ViewCreateInfo.subresourceRange.layerCount = 1;

	VkImageView t_ImageView;

	if (vkCreateImageView(a_LogicalDevice, &t_ViewCreateInfo, nullptr, &t_ImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Failed to create Image View!");
	}

	return t_ImageView;
}

const VkImage& Image::GetImage() const
{
	return m_Image;
}

const VkImageView Image::GetImageView()
{
	return m_ImageView;
}

void Image::TransitionImageLayout(VkFormat a_Format, VkImageLayout a_OldLayout, VkImageLayout a_NewLayout,
                                  VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue, const VkDevice& a_LogicalDevice)
{
	VkCommandBuffer t_CmdBuffer = BeginSingleTimeCommand(a_CommandPool, a_LogicalDevice);

	VkPipelineStageFlags t_SourceStage;
	VkPipelineStageFlags t_DestinationStage;

	const VkImageMemoryBarrier t_MemoryBarrier = GenImageBarrier(a_OldLayout, a_NewLayout, t_SourceStage, t_DestinationStage);

	vkCmdPipelineBarrier(t_CmdBuffer, 
		t_SourceStage, t_DestinationStage, 
		0, 
		0, nullptr, 
		0, nullptr, 
		1, &t_MemoryBarrier);

	EndSingleTimeCommands(t_CmdBuffer, a_GraphicsQueue, a_LogicalDevice, a_CommandPool);
}

/// <summary>	Allocate image memory. </summary>
/// <exception cref="std::runtime_error">	Raised when a image memory could not be allocated.</exception>
/// <param name="a_Device">		  	The device.</param>
/// <param name="a_PropertyFlags">	The property flags.</param>

void Image::AllocateImageMemory(Device a_Device, VkMemoryPropertyFlags a_PropertyFlags)
{
	// query memory requirements
	VkMemoryRequirements t_MemoryRequirements;
	vkGetImageMemoryRequirements(a_Device.GetLogicalDevice(), m_Image, &t_MemoryRequirements);

	// allocate memory
	VkMemoryAllocateInfo t_AllocateInfo = {};
	t_AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	t_AllocateInfo.allocationSize = t_MemoryRequirements.size;
	t_AllocateInfo.memoryTypeIndex = Buffer::GetMemoryType(a_Device, t_MemoryRequirements.memoryTypeBits, a_PropertyFlags);

	if (vkAllocateMemory(a_Device.GetLogicalDevice(), &t_AllocateInfo, nullptr, &m_ImageMemory))
	{
		throw std::runtime_error("Error! Could not allocate image memory!");
	}
}

VkImageMemoryBarrier Image::GenImageBarrier(VkImageLayout a_OldLayout, VkImageLayout a_NewLayout,
                                            VkPipelineStageFlags& a_SourceStage,
                                            VkPipelineStageFlags& a_DestinationStage) const
{
	VkImageMemoryBarrier t_MemoryBarrier = {};
	t_MemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	t_MemoryBarrier.oldLayout = a_OldLayout;
	t_MemoryBarrier.newLayout = a_NewLayout;

	// no transfer of ownership between queues required
	t_MemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	t_MemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	t_MemoryBarrier.image = m_Image;
	// sub-resource config
	t_MemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	t_MemoryBarrier.subresourceRange.baseMipLevel = 0;
	t_MemoryBarrier.subresourceRange.levelCount = 1;
	t_MemoryBarrier.subresourceRange.baseArrayLayer = 0;
	t_MemoryBarrier.subresourceRange.layerCount = 1;

	// if the transfer write does not need to wait
	if (a_OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && a_NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		t_MemoryBarrier.srcAccessMask = 0;
		t_MemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		a_SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		a_DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	// else if operation is a shader read, wait for transfer writes
	else if (a_OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && a_NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		t_MemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		t_MemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		a_SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		a_DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("Error! Layout Transition not supported!");
	}

	return t_MemoryBarrier;
}
