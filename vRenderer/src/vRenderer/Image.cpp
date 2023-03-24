#include "pch.h"
#include "vRenderer/Image.h"
#include "vulkan/vulkan_core.h"
#include "vRenderer/Device.h"
#include "vRenderer/Buffer/Buffer.h"
#include <stdexcept>

Image::Image()
= default;

Image::~Image()
= default;

void Image::CreateImage(const Device& a_Device, uint32_t a_Width, uint32_t a_Height, VkFormat a_Format, VkImageTiling a_Tiling,
                        VkImageUsageFlags a_UsageFlags, VkMemoryPropertyFlags a_PropertyFlags)
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
}

const VkImage& Image::GetImage()
{
	return m_Image;
}

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
