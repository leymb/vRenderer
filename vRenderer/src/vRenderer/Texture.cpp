#include "pch.h"
#include "vRenderer/Texture.h"
#include "vRenderer/Buffer/Buffer.h"
#include "vRenderer/Device.h"


#include "stb_image/stb_image.h"

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

	// create and fill staging buffer
	Buffer t_StagingBuffer;

	t_StagingBuffer.CreateBuffer(t_ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);

	t_StagingBuffer.FillBuffer(t_ImageSize, a_Device.GetLogicalDevice(), t_Pixels);

	// unload image
	stbi_image_free(t_Pixels);

	// create Image
	m_Texture.CreateImage(a_Device, t_TextureWidth, t_TextureHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
	                      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
	                      VK_IMAGE_ASPECT_COLOR_BIT);

	// transition image layout safely using image memory barrier
	m_Texture.TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
	                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, a_CommandPool, a_GraphicsQueue,
	                                a_Device.GetLogicalDevice());

	t_StagingBuffer.CopyBufferToImage(m_Texture.GetImage(), static_cast<uint32_t>(t_TextureWidth),
	                                  static_cast<uint32_t>(t_TextureHeight), a_CommandPool,
	                                  a_Device.GetLogicalDevice(), a_GraphicsQueue);

	// prepare for sampling in the shader
	m_Texture.TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, a_CommandPool, a_GraphicsQueue,
	                                a_Device.GetLogicalDevice());

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
	t_SamplerCreateInfo.maxLod = 0.0f;

	if (vkCreateSampler(a_Device.GetLogicalDevice(), &t_SamplerCreateInfo,nullptr, &m_TextureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Failed to create Texture Sampler!");
	}
}

const VkImageView& Texture::GetImageView()
{
	return m_Texture.GetImageView();
}

const VkSampler& Texture::GetSampler()
{
	return m_TextureSampler;
}
