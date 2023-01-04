#include "pch.h"
#include "vRenderer/VertexBuffer.h"
#include "vRenderer/helper_structs/Vertex.h"
#include "vRenderer/Device.h"
#include <stdexcept>

VertexBuffer::VertexBuffer()
{
}

void VertexBuffer::CreateVertexBuffer(std::vector<Vertex>& a_Vertices, const Device& a_Device)
{
	VkBufferCreateInfo t_BufferCreateInfo = {};
	t_BufferCreateInfo.sType =  VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	t_BufferCreateInfo.size = sizeof(a_Vertices[0]) * a_Vertices.size();
	t_BufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	t_BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(a_Device.GetLogicalDevice(),&t_BufferCreateInfo, nullptr, &m_Buffer))
	{
		throw std::runtime_error("Error: Could not create Vertex VertexBuffer!");
	}

	AllocateMemory(a_Device);

	vkBindBufferMemory(a_Device.GetLogicalDevice(), m_Buffer, m_Memory, 0);

	FillVertexBuffer(t_BufferCreateInfo, a_Device.GetLogicalDevice(), a_Vertices);
}

void VertexBuffer::DestroyBuffer(const VkDevice& a_LogicalDevice)
{
	vkDestroyBuffer(a_LogicalDevice, m_Buffer, nullptr);
	vkFreeMemory(a_LogicalDevice, m_Memory, nullptr);
}

void VertexBuffer::AllocateMemory(const Device& a_Device)
{
	VkMemoryRequirements t_MemoryRequirements = GetMemoryRequirements(a_Device.GetLogicalDevice());

	VkMemoryAllocateInfo t_AllocateInfo = {};
	t_AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	t_AllocateInfo.allocationSize = t_MemoryRequirements.size;
	t_AllocateInfo.memoryTypeIndex = GetMemoryType(a_Device, t_MemoryRequirements.memoryTypeBits,
	                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(a_Device.GetLogicalDevice(), &t_AllocateInfo, nullptr, &m_Memory))
	{
		throw std::runtime_error("Error: Could not allocate Vertex VertexBuffer Memory!");
	}

}

/// <summary>	Fills the vertex buffer with vertex data. </summary>
/// <param name="a_BufferInfo">   	[in,out] Information describing the buffer.</param>
/// <param name="a_LogicalDevice">	The logical device.</param>
/// <param name="a_Vertices">	  	[in,out] The vertices.</param>

void VertexBuffer::FillVertexBuffer(VkBufferCreateInfo& a_BufferInfo, const VkDevice& a_LogicalDevice,
                              std::vector<Vertex>& a_Vertices)
{
	vkMapMemory(a_LogicalDevice, m_Memory, 0, a_BufferInfo.size, 0, &m_Data);
	memcpy(m_Data, a_Vertices.data(), static_cast<size_t>(a_BufferInfo.size));
	vkUnmapMemory(a_LogicalDevice, m_Memory);
}

const VkBuffer& VertexBuffer::GetBuffer() const
{
	return m_Buffer;
}

VkMemoryRequirements VertexBuffer::GetMemoryRequirements(const VkDevice& a_LogicalDevice) const
{
	VkMemoryRequirements t_MemoryRequirements;
	vkGetBufferMemoryRequirements(a_LogicalDevice, m_Buffer, &t_MemoryRequirements);

	return t_MemoryRequirements;
}

uint32_t VertexBuffer::GetMemoryType(const Device& a_Device, uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties)
{
	// get available types of memory
	VkPhysicalDeviceMemoryProperties t_MemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(a_Device.GetPhysicalDevice(), &t_MemoryProperties);

	// find suitable memory type
	for (uint32_t i = 0; i < t_MemoryProperties.memoryTypeCount; i++)
	{
		if (a_TypeFilter & (1 << i) 
			&& (t_MemoryProperties.memoryTypes[i].propertyFlags & a_Properties) == a_Properties)
		{
			return i;
		}
	}

	// if none can be found, return runtime error
	throw std::runtime_error("Error: Could not find suitable memory type!");
}
