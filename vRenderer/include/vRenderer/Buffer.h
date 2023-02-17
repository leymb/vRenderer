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

protected:

	VkMemoryRequirements GetMemoryRequirements(const VkDevice& a_LogicalDevice) const;
	static uint32_t GetMemoryType(const Device& a_Device, uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties);
	void AllocateMemory(const Device& a_Device, VkMemoryPropertyFlags a_Properties);

	void FillBuffer(VkDeviceSize a_BufferSize, const VkDevice& a_LogicalDevice,
	                void* a_Data);

	VkBuffer m_Buffer;
	VkDeviceMemory m_Memory;
	void* m_Data;
};