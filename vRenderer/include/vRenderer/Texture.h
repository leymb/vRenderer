#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"

// todo remove parenting to buffer
class Texture : private Buffer
{
public:
	Texture();
	~Texture();

	void DestroyTexture(VkDevice a_LogicalDevice);

	void CreateTextureFromImage(const char* a_FilePath, const Device& a_Device, VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue);

private:

	Image m_Texture;
};

