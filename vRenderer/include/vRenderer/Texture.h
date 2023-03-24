#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"

// todo remove parenting to buffer
class Texture : private Buffer
{
public:
	Texture();
	~Texture();

	void CreateTextureFromImage(const char* a_FilePath, const Device& a_Device);

private:

	Image m_Texture;
};

