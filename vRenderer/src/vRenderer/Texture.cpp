#include "pch.h"
#include "vRenderer/Texture.h"
#include "vRenderer/Buffer/Buffer.h"
#include "vRenderer/Device.h"


#include "stb_image/stb_image.h"

Texture::Texture()
= default;

Texture::~Texture()
= default;

void Texture::CreateTextureFromImage(const char* a_FilePath, const Device& a_Device)
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
	                      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}
