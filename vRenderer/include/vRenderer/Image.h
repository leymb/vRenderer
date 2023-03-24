#pragma once

class Image
{
public:
	Image();
	~Image();

	void CreateImage(const Device& a_Device, uint32_t a_Width, uint32_t a_Height, VkFormat a_Format, VkImageTiling a_Tiling,
	                 VkImageUsageFlags a_UsageFlags, VkMemoryPropertyFlags a_PropertyFlags);
	const VkImage& GetImage();

private:
	void AllocateImageMemory(Device a_Device, VkMemoryPropertyFlags a_PropertyFlags);

	VkImage m_Image;
	VkDeviceMemory m_ImageMemory;
};

