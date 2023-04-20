#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"
#include "Buffer/Buffer.h"

// todo remove parenting to buffer
class Texture : private Buffer
{
public:
	Texture();
	~Texture();

	/// <summary>	Destroys the texture and the Image contained within. </summary>
	/// <param name="a_LogicalDevice">	The logical device.</param>

	void DestroyTexture(VkDevice a_LogicalDevice);

	/// <summary>	Loads image data from a file and automatically loads it into an Image object to create a texture. </summary>
	/// <param name="a_FilePath">	  	Full pathname of the file.</param>
	/// <param name="a_Device">		  	The device.</param>
	/// <param name="a_CommandPool">  	[in,out] The command pool.</param>
	/// <param name="a_GraphicsQueue">	Graphics Queue.</param>

	void CreateTextureFromImage(const char* a_FilePath, const Device& a_Device, VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue);

	/// <summary>	Creates a sampler to be used in texture sampling. </summary>
	/// <param name="a_Device">	[in,out] The device.</param>

	void CreateTextureSampler(const Device& a_Device);

	const VkImageView& GetImageView() const;

	const VkSampler& GetSampler() const;

private:

	Image m_Texture;

	VkSampler m_TextureSampler;
};

