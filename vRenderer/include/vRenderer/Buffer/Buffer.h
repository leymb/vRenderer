#pragma once
#include "vulkan/vulkan_core.h"
#include <stdexcept>

class Device;
class Buffer
{
public:
	Buffer();

	/// <summary>	Creates a buffer and allocates memory for it. </summary>
	/// <param name="a_Size">		  	The size of the memory.</param>
	/// <param name="a_UsageFlag">	  	The usage flags.</param>
	/// <param name="a_PropertyFlags">	The property flags.</param>
	/// <param name="a_Device">		  	The logical device.</param>

	void CreateBuffer(VkDeviceSize a_Size, VkBufferUsageFlags a_UsageFlag, VkMemoryPropertyFlags a_PropertyFlags, const Device& a_Device);

	/// <summary>
	/// 	Destroys the buffer using vkDestroyBuffer and also frees the allocated memory.
	/// </summary>
	/// <param name="a_LogicalDevice">	The logical device.</param>

	void DestroyBuffer(const VkDevice& a_LogicalDevice);

	const VkBuffer& GetBuffer() const;

	/// <summary>	Copies the contents of this buffer into the destination buffer. </summary>
	/// <param name="a_DstBuffer">	  	Destination Buffer the data is copied into.</param>
	/// <param name="a_LogicalDevice">	The logical device.</param>
	/// <param name="a_DeviceSize">   	Size of the device.</param>
	/// <param name="a_GraphicsQueue">	Queue used to execute the copy command.</param>
	/// <param name="a_CommandPool">  	The command pool that should execute the transfer commands.</param>

	void CopyInto(VkBuffer a_DstBuffer, VkDevice a_LogicalDevice, VkDeviceSize a_DeviceSize, VkQueue a_GraphicsQueue,
	              VkCommandPool a_CommandPool);

	/// <summary>	Fills the buffer with the provided data. </summary>
	/// <param name="a_BufferSize">   	Size of the buffer.</param>
	/// <param name="a_LogicalDevice">	The logical device.</param>
	/// <param name="a_Data">		  	[in,out] If non-null, the data.</param>

	void FillBuffer(VkDeviceSize a_BufferSize, const VkDevice& a_LogicalDevice,
	                void* a_Data);

protected:

	VkMemoryRequirements GetMemoryRequirements(const VkDevice& a_LogicalDevice) const;
	static uint32_t GetMemoryType(const Device& a_Device, uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties);
	void AllocateMemory(const Device& a_Device, VkMemoryPropertyFlags a_Properties);

	VkBuffer m_Buffer;
	VkDeviceMemory m_Memory;
	void* m_Data;
};