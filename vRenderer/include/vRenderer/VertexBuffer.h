#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>

class Device;
struct Vertex;

class VertexBuffer
{
public:
	VertexBuffer();

	/// <summary>	Creates a vertex buffer and allocates memory for it. </summary>
	/// <param name="a_Vertices">	  	[in,out] The vertices.</param>
	/// <param name="a_LogicalDevice">	[in,out] The logical device.</param>

	void CreateVertexBuffer(std::vector<Vertex>& a_Vertices, const Device& a_Device);

	/// <summary>
	/// 	Destroys the buffer using vkDestroyBuffer and also frees the allocated memory.
	/// </summary>
	/// <param name="a_LogicalDevice">	The logical device.</param>

	void DestroyBuffer(const VkDevice& a_LogicalDevice);

	const VkBuffer& GetBuffer() const;

private:
	VkMemoryRequirements GetMemoryRequirements(const VkDevice& a_LogicalDevice) const;
	uint32_t GetMemoryType(const Device& a_Device, uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties);
	void AllocateMemory(const Device& a_Device);
	void FillVertexBuffer(VkBufferCreateInfo& a_BufferInfo, const VkDevice& a_LogicalDevice,
	                      std::vector<Vertex>& a_Vertices);

	VkBuffer m_Buffer;
	VkDeviceMemory m_Memory;
	void* m_Data;
};

