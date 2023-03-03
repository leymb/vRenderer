#include "pch.h"
#include "vRenderer/Buffer/VertexBuffer.h"
#include "vRenderer/helper_structs/Vertex.h"
#include "vRenderer/Device.h"

VertexBuffer::VertexBuffer()
= default;

VertexBuffer::~VertexBuffer()
= default;

void VertexBuffer::CreateVertexBuffer(std::vector<Vertex>& a_Vertices, const Device& a_Device, VkQueue a_GraphicsQueue,
	              VkCommandPool a_CommandPool)
{
	VkDeviceSize t_BufferSize = sizeof(a_Vertices[0]) * a_Vertices.size();

	// create staging buffer
	m_StagingBuffer.CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);

	// fill staging buffer with vertices
	m_StagingBuffer.FillBuffer(t_BufferSize, a_Device.GetLogicalDevice(), a_Vertices.data());

	// create Vertex Buffer
	CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, a_Device);

	// copy data from staging buffer into vertex buffer
	m_StagingBuffer.CopyInto(m_Buffer, a_Device.GetLogicalDevice(), t_BufferSize, a_GraphicsQueue, a_CommandPool);
}