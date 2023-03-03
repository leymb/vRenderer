#include "pch.h"
#include "vRenderer/Buffer/IndexBuffer.h"
#include "vRenderer/Device.h"

IndexBuffer::IndexBuffer()
= default;

IndexBuffer::~IndexBuffer()
= default;

void IndexBuffer::CreateIndexBuffer(std::vector<uint16_t> a_Indices, const Device& a_Device, VkQueue a_GraphicsQueue,
	              VkCommandPool a_CommandPool)
{
	VkDeviceSize t_BufferSize = sizeof(a_Indices[0]) * a_Indices.size();

	// create staging buffer
	Buffer t_StagingBuffer = {};
	t_StagingBuffer.CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);

	// fill staging buffer with indices
	t_StagingBuffer.FillBuffer(t_BufferSize, a_Device.GetLogicalDevice(), a_Indices.data());

	// create index Buffer
	CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, a_Device);

	// copy data from staging buffer into index buffer
	t_StagingBuffer.CopyInto(m_Buffer, a_Device.GetLogicalDevice(), t_BufferSize, a_GraphicsQueue, a_CommandPool);

	// free buffer
	t_StagingBuffer.DestroyBuffer(a_Device.GetLogicalDevice());
}