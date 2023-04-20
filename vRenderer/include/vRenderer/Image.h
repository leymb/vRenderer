#pragma once
#include "Device.h"

class Image
{
public:
	Image();
	~Image();

	/// <summary>
	/// 	Creates a VkImage and allocates device memory for it based on the provided parameters.
	/// </summary>
	/// <param name="a_Device">		  	The device.</param>
	/// <param name="a_Width">		  	The width.</param>
	/// <param name="a_Height">		  	The height.</param>
	/// <param name="a_Format">		  	Describes the format to use.</param>
	/// <param name="a_Tiling">		  	The tiling.</param>
	/// <param name="a_UsageFlags">   	The usage flags.</param>
	/// <param name="a_PropertyFlags">	The property flags.</param>

	void CreateImage(const Device& a_Device, uint32_t a_Width, uint32_t a_Height, VkFormat a_Format, VkImageTiling a_Tiling,
	                 VkImageUsageFlags a_UsageFlags, VkMemoryPropertyFlags a_PropertyFlags, VkImageAspectFlags a_ImageAspectFlag);

	/// <summary>	Destroys the VkImage and frees its memory. </summary>
	/// <param name="a_LogicalDevice">	The logical device.</param>

	void DestroyImage(VkDevice a_LogicalDevice);

	/// <summary>	Creates an image view for the VkImage contained within this object. </summary>
	/// <param name="a_Format">		  	Describes the format to use.</param>
	/// <param name="a_LogicalDevice">	The logical device.</param>
	/// <returns>	The new image view. </returns>

	VkImageView CreateImageView(VkFormat a_Format, VkDevice a_LogicalDevice, VkImageAspectFlags a_AspectFlag) const;

	/// <summary>	Gets the raw VkImage. </summary>
	/// <returns>	The VkImage. </returns>

	const VkImage& GetImage() const;

	/// <summary>	Gets image view to the VkImage contained within this object. </summary>
	/// <returns>	The image view. </returns>

	const VkImageView GetImageView() const;

	void TransitionImageLayout(VkFormat a_Format, VkImageLayout a_OldLayout, VkImageLayout a_NewLayout,
	                           VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue, const VkDevice& a_LogicalDevice);

private:
	void AllocateImageMemory(Device a_Device, VkMemoryPropertyFlags a_PropertyFlags);

	VkImageMemoryBarrier GenImageBarrier(VkImageLayout a_OldLayout, VkImageLayout a_NewLayout,
	                                     VkPipelineStageFlags& a_SourceStage,
	                                     VkPipelineStageFlags& a_DestinationStage) const;

	VkImage m_Image;
	VkDeviceMemory m_ImageMemory;

	VkImageView m_ImageView;
};

