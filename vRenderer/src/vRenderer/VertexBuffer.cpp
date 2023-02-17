#include "pch.h"
#include "vRenderer/VertexBuffer.h"
#include "vRenderer/helper_structs/Vertex.h"
#include "vRenderer/Device.h"

VertexBuffer::VertexBuffer()
= default;

VertexBuffer::~VertexBuffer()
= default;

void VertexBuffer::CreateVertexBuffer(std::vector<Vertex>& a_Vertices, const Device& a_Device)
{
	VkDeviceSize t_BufferSize = sizeof(a_Vertices[0]) * a_Vertices.size();
	CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);

	FillBuffer(t_BufferSize, a_Device.GetLogicalDevice(), a_Vertices.data());
}