#include "pch.h"
#include "vRenderer/Buffer/UniformBuffer.h"

#include <array>

#include "vRenderer/helper_structs/UniformBufferObject.h"
#include "vRenderer/Device.h"

UniformBuffer::UniformBuffer()
= default;

UniformBuffer::~UniformBuffer()
= default;

void UniformBuffer::CreateUniformBuffer(const Device& a_Device)
{
	const VkDeviceSize t_BufferSize = sizeof(UniformBufferObject);

	CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);

	// persistent mapping (get a pointer to write data to later)
	vkMapMemory(a_Device.GetLogicalDevice(), m_Memory, 0, t_BufferSize, 0, &m_AccessPointer);
}

void UniformBuffer::CreateUniformBuffer(const Device& a_Device, VkDeviceSize a_BufferSize)
{
	CreateBuffer(a_BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, a_Device);

	// persistent mapping (get a pointer to write data to later)
	vkMapMemory(a_Device.GetLogicalDevice(), m_Memory, 0, a_BufferSize, 0, &m_AccessPointer);
}

VkDescriptorSetLayout UniformBuffer::CreateDescriptorSetLayout(const VkDevice a_Device)
{
	VkDescriptorSetLayout t_DescriptorSetLayout;

	// UBO Binding
	VkDescriptorSetLayoutBinding t_UniformBufferObjectBinding;
	t_UniformBufferObjectBinding.binding  = 0;
	t_UniformBufferObjectBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	t_UniformBufferObjectBinding.descriptorCount = 1;
	t_UniformBufferObjectBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	t_UniformBufferObjectBinding.pImmutableSamplers = nullptr;

	// Sampler Binding
	VkDescriptorSetLayoutBinding t_SamplerLayoutBinding;
	t_SamplerLayoutBinding.binding = 1;
	t_SamplerLayoutBinding.descriptorCount = 1;
	t_SamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	t_SamplerLayoutBinding.pImmutableSamplers = nullptr;
	t_SamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> t_Bindings = {t_UniformBufferObjectBinding, t_SamplerLayoutBinding};

	// create info
	VkDescriptorSetLayoutCreateInfo t_DescriptorSetLayoutCreateInfo = {};
	t_DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	t_DescriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(t_Bindings.size());
	t_DescriptorSetLayoutCreateInfo.pBindings = t_Bindings.data();

	if (vkCreateDescriptorSetLayout(a_Device, &t_DescriptorSetLayoutCreateInfo, nullptr, &t_DescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Failed to create Uniform Buffer descriptor set layout!");
	}

	return t_DescriptorSetLayout;
}

void UniformBuffer::FillBuffer(const UniformBufferObject& a_Ubo)
{
	memcpy(m_AccessPointer, &a_Ubo, sizeof(a_Ubo));
}