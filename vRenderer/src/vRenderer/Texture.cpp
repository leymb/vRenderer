#include "pch.h"
#include "vRenderer/Texture.h"
#include "vRenderer/Buffer/Buffer.h"
#include "vRenderer/Device.h"
#include "vRenderer/helpers/VulkanHelpers.h"


#include "stb_image/stb_image.h"
#include <cmath>

Texture::Texture()
= default;

Texture::~Texture()
= default;

void Texture::DestroyTexture(VkDevice a_LogicalDevice)
{
	m_Texture.DestroyImage(a_LogicalDevice);
	vkDestroySampler(a_LogicalDevice, m_TextureSampler, nullptr);
}

void Texture::CreateTextureFromImage(const char* a_FilePath, const Device& a_Device,VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue)
{
	// load image
	int t_TextureWidth = 0;
	int t_TextureHeight = 0;
	int t_TextureChannels = 0;

	stbi_uc* t_Pixels = stbi_load(a_FilePath, &t_TextureWidth, &t_TextureHeight, &t_TextureChannels, STBI_rgb_alpha);

	// calculate buffer size
	VkDeviceSize t_ImageSize = t_TextureWidth * t_TextureHeight * 4;

	if (!t_Pixels)
	{
		throw std::runtime_error("Error! Could not load texture!");
	}

	// calculate mip levels

	uint32_t t_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(t_TextureWidth, t_TextureHeight)))) + 1;

	// create and fill staging buffer
	Buffer t_StagingBuffer;

	t_StagingBuffer.CreateBuffer(t_ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);

	t_StagingBuffer.FillBuffer(t_ImageSize, a_Device.GetLogicalDevice(), t_Pixels);

	// unload image
	stbi_image_free(t_Pixels);

	// create Image
	m_Texture.CreateImage(a_Device, t_TextureWidth, t_TextureHeight, t_MipLevels, VK_SAMPLE_COUNT_1_BIT,
	                      VK_FORMAT_R8G8B8A8_SRGB,
	                      VK_IMAGE_TILING_OPTIMAL,
	                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
	                      VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	// transition image layout safely using image memory barrier
	m_Texture.TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	                                a_CommandPool, a_GraphicsQueue, a_Device.GetLogicalDevice(), t_MipLevels);

	t_StagingBuffer.CopyBufferToImage(m_Texture.GetImage(), static_cast<uint32_t>(t_TextureWidth),
	                                  static_cast<uint32_t>(t_TextureHeight), a_CommandPool,
	                                  a_Device.GetLogicalDevice(), a_GraphicsQueue);

	GenMipMaps(t_TextureWidth, t_TextureHeight, t_MipLevels, a_CommandPool, a_Device, a_GraphicsQueue, VK_FORMAT_R8G8B8A8_SRGB);


	// cleanup 
	t_StagingBuffer.DestroyBuffer(a_Device.GetLogicalDevice());
}

void Texture::CreateTextureSampler(const Device& a_Device)
{
	VkSamplerCreateInfo t_SamplerCreateInfo = {};
	t_SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	t_SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	t_SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;

	// not that relevant because I do not sample outside of the image
	t_SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	t_SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	t_SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	// anisotropy
	VkPhysicalDeviceProperties t_DeviceProperties = {};
	vkGetPhysicalDeviceProperties(a_Device.GetPhysicalDevice(), &t_DeviceProperties);

	t_SamplerCreateInfo.anisotropyEnable = VK_TRUE;
	t_SamplerCreateInfo.maxAnisotropy = t_DeviceProperties.limits.maxSamplerAnisotropy;

	t_SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	t_SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	t_SamplerCreateInfo.compareEnable = VK_FALSE;
	t_SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	// mipmapping
	t_SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	t_SamplerCreateInfo.mipLodBias = 0.0f;
	t_SamplerCreateInfo.minLod = 0.0f;
	t_SamplerCreateInfo.maxLod = static_cast<float>(m_Texture.GetMipLevels());

	if (vkCreateSampler(a_Device.GetLogicalDevice(), &t_SamplerCreateInfo,nullptr, &m_TextureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Failed to create Texture Sampler!");
	}
}

const VkImageView& Texture::GetImageView() const
{
	return m_Texture.GetImageView();
}

const VkSampler& Texture::GetSampler() const
{
	return m_TextureSampler;
}

void Texture::GenMipMaps(int32_t a_TexWidth, int32_t a_TexHeight, uint32_t a_MipLevels, VkCommandPool& a_CommandPool,
                         const Device& a_Device, const VkQueue& a_GraphicsQueue, VkFormat a_ImageFormat)
{
	// check if blitting is supported
	VkFormatProperties t_FormatProperties;
	vkGetPhysicalDeviceFormatProperties(a_Device.GetPhysicalDevice(), a_ImageFormat, &t_FormatProperties);

	if (!(t_FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		throw std::runtime_error("Error! Texture image format does not support blitting!");
	}

	VkCommandBuffer t_CommandBuffer = BeginSingleTimeCommand(a_CommandPool, a_Device.GetLogicalDevice());
	VkImageMemoryBarrier t_Barrier = {};
	t_Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	t_Barrier.image = m_Texture.GetImage();
	t_Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	t_Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	t_Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	t_Barrier.subresourceRange.baseArrayLayer = 0;
	t_Barrier.subresourceRange.layerCount = 1;
	t_Barrier.subresourceRange.levelCount = 1;

	int32_t t_MipWidth = a_TexWidth;
	int32_t t_MipHeight = a_TexHeight;

	for (uint32_t i = 1; i < a_MipLevels; i++)
	{
		// transfer image to image layout optimal for blitting
		t_Barrier.subresourceRange.baseMipLevel = i - 1;
		t_Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		t_Barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		t_Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		t_Barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(t_CommandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &t_Barrier);

		// actually generate mip
		VkImageBlit t_Blit = {};
		t_Blit.srcOffsets[0] = {0, 0, 0};
		t_Blit.srcOffsets[1] = {t_MipWidth, t_MipHeight, 1};
		t_Blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		t_Blit.srcSubresource.mipLevel = i - 1;
		t_Blit.srcSubresource.baseArrayLayer = 0;
		t_Blit.srcSubresource.layerCount = 1;
		t_Blit.dstOffsets[0] = {0, 0, 0};
		t_Blit.dstOffsets[1] = { t_MipWidth > 1 ? t_MipWidth / 2 : 1, t_MipHeight > 1 ? t_MipHeight / 2 : 1, 1};
		t_Blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		t_Blit.dstSubresource.mipLevel = i;
		t_Blit.dstSubresource.baseArrayLayer = 0;
		t_Blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(t_CommandBuffer,
			m_Texture.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_Texture.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &t_Blit,
			VK_FILTER_LINEAR
		);

		// transfer image to shader read only optimals
		t_Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		t_Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		t_Barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		t_Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(t_CommandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &t_Barrier);

		if (t_MipWidth > 1) t_MipWidth /= 2;
		if (t_MipHeight > 1) t_MipHeight /= 2;
	}

	t_Barrier.subresourceRange.baseMipLevel = a_MipLevels - 1;
	t_Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	t_Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	t_Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	t_Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(t_CommandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &t_Barrier);

	EndSingleTimeCommands(t_CommandBuffer, a_GraphicsQueue, a_Device.GetLogicalDevice(), a_CommandPool);
}
