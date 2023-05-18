#include "pch.h"
#include "../include/vRenderer/Buffer/ShaderStorageBuffer.h"
#include <array>

ShaderStorageBuffer::ShaderStorageBuffer()
{
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
}

void ShaderStorageBuffer::Create(void* a_Data, VkDeviceSize a_BufferSize, const Device& a_Device,
	VkQueue a_GraphicsQueue, VkCommandPool a_CommandPool)
{
	// staging buffer (CPU side) to hold particle data
	Buffer t_StagingBuffer;
	t_StagingBuffer.CreateBuffer(a_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);
	t_StagingBuffer.FillBuffer(a_BufferSize, a_Device.GetLogicalDevice(), a_Data);

	// create actual buffer
	CreateBuffer(a_BufferSize,
	             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
	             VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, a_Device);

	// copy data from staging buffer to actual buffer on the device
	t_StagingBuffer.CopyInto(m_Buffer, a_Device.GetLogicalDevice(), a_BufferSize, a_GraphicsQueue, a_CommandPool);

	t_StagingBuffer.DestroyBuffer(a_Device.GetLogicalDevice());
}

void ShaderStorageBuffer::CreateWithStagingBuffer(const Buffer& a_StagingBuffer, const VkDeviceSize a_BufferSize, const Device& a_Device,
	VkQueue a_GraphicsQueue, VkCommandPool a_CommandPool)
{
	// create actual buffer
	CreateBuffer(a_BufferSize,
	             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
	             VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, a_Device);

	// copy data from staging buffer to actual buffer on the device
	a_StagingBuffer.CopyInto(m_Buffer, a_Device.GetLogicalDevice(), a_BufferSize, a_GraphicsQueue, a_CommandPool);
}

VkDescriptorSetLayout ShaderStorageBuffer::CreateDescriptorSetLayout(const VkDevice a_Device)
{
	std::array<VkDescriptorSetLayoutBinding, 3> t_LayoutBindings = {};
	t_LayoutBindings[0].binding = 0;
	t_LayoutBindings[0].descriptorCount = 1;
	t_LayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	t_LayoutBindings[0].pImmutableSamplers = nullptr;
	t_LayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	t_LayoutBindings[1].binding = 1;
	t_LayoutBindings[1].descriptorCount = 1;
	t_LayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	t_LayoutBindings[1].pImmutableSamplers = nullptr;
	t_LayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	t_LayoutBindings[2].binding = 2;
	t_LayoutBindings[2].descriptorCount = 1;
	t_LayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	t_LayoutBindings[2].pImmutableSamplers = nullptr;
	t_LayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo t_CreateInfo = {};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	t_CreateInfo.bindingCount = 3;
	t_CreateInfo.pBindings = t_LayoutBindings.data();

	VkDescriptorSetLayout t_ComputeDescriptorSetLayout;
	if (vkCreateDescriptorSetLayout(a_Device, &t_CreateInfo, nullptr, &t_ComputeDescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Failed to create Descriptor Set Layout for Shader Storage Buffer!");
	}

	return t_ComputeDescriptorSetLayout;
}
