#include "pch.h"
#include "vRenderer/Buffer.h"
#include "vRenderer/helper_structs/Vertex.h"
#include "vRenderer/Device.h"

Buffer::Buffer()
{
}

void Buffer::CreateBuffer(VkDeviceSize a_Size, VkBufferUsageFlags a_UsageFlag, VkMemoryPropertyFlags a_PropertyFlags, const Device& a_Device)
{
	VkBufferCreateInfo t_CreateInfo = {};
	t_CreateInfo.size = a_Size;
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	t_CreateInfo.usage = a_UsageFlag;
	t_CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(a_Device.GetLogicalDevice(), &t_CreateInfo, nullptr, &m_Buffer))
	{
		throw std::runtime_error("Error: Could not create Vertex VertexBuffer!");
	}

	const VkMemoryRequirements t_MemoryRequirements = GetMemoryRequirements(a_Device.GetLogicalDevice());

	VkMemoryAllocateInfo t_AllocateInfo = {};
	t_AllocateInfo.sType =  VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	t_AllocateInfo.allocationSize = t_MemoryRequirements.size;
	t_AllocateInfo.memoryTypeIndex = GetMemoryType(a_Device, t_MemoryRequirements.memoryTypeBits, a_PropertyFlags);

	if (vkAllocateMemory(a_Device.GetLogicalDevice(), &t_AllocateInfo, nullptr, &m_Memory))
	{
		throw std::runtime_error("Error: Failed to allocate buffer memory!");
	}

	vkBindBufferMemory(a_Device.GetLogicalDevice(), m_Buffer, m_Memory, 0);
}

void Buffer::DestroyBuffer(const VkDevice& a_LogicalDevice)
{
	vkDestroyBuffer(a_LogicalDevice, m_Buffer, nullptr);
	vkFreeMemory(a_LogicalDevice, m_Memory, nullptr);
}

const VkBuffer& Buffer::GetBuffer() const
{
	return m_Buffer;
}

void Buffer::CopyInto(VkBuffer a_DstBuffer, VkDevice a_LogicalDevice, VkDeviceSize a_DeviceSize, VkQueue a_GraphicsQueue,
	              VkCommandPool a_CommandPool)
{
	// create temporary command buffer
	VkCommandBufferAllocateInfo t_CmdBufferAllocateInfo = {};

	t_CmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	t_CmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	t_CmdBufferAllocateInfo.commandPool = a_CommandPool;
	t_CmdBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer t_CmdBuffer;
	vkAllocateCommandBuffers(a_LogicalDevice, &t_CmdBufferAllocateInfo, &t_CmdBuffer);

	// begin recording command buffer
	VkCommandBufferBeginInfo t_CmdBeginInfo = {};
	t_CmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	t_CmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(t_CmdBuffer, &t_CmdBeginInfo);

	// copy data
	VkBufferCopy t_CopyRegion = {};
	t_CopyRegion.srcOffset = 0;
	t_CopyRegion.dstOffset = 0;
	t_CopyRegion.size = a_DeviceSize;

	vkCmdCopyBuffer(t_CmdBuffer, m_Buffer, a_DstBuffer, 1, &t_CopyRegion);

	// end recording command buffer
	vkEndCommandBuffer(t_CmdBuffer);

	// submit command buffer to complete copying
	VkSubmitInfo t_CmdSubmitInfo = {};
	t_CmdSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	t_CmdSubmitInfo.commandBufferCount = 1;
	t_CmdSubmitInfo.pCommandBuffers = &t_CmdBuffer;

	vkQueueSubmit(a_GraphicsQueue, 1, &t_CmdSubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(a_GraphicsQueue);

	// cleanup 
	vkFreeCommandBuffers(a_LogicalDevice, a_CommandPool, 1, &t_CmdBuffer);
}

VkMemoryRequirements Buffer::GetMemoryRequirements(const VkDevice& a_LogicalDevice) const
{
	VkMemoryRequirements t_MemoryRequirements;
	vkGetBufferMemoryRequirements(a_LogicalDevice, m_Buffer, &t_MemoryRequirements);

	return t_MemoryRequirements;
}

uint32_t Buffer::GetMemoryType(const Device& a_Device, uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties)
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

void Buffer::AllocateMemory(const Device& a_Device, VkMemoryPropertyFlags a_Properties)
{
	VkMemoryRequirements t_MemoryRequirements = GetMemoryRequirements(a_Device.GetLogicalDevice());

	VkMemoryAllocateInfo t_AllocateInfo = {};
	t_AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	t_AllocateInfo.allocationSize = t_MemoryRequirements.size;
	t_AllocateInfo.memoryTypeIndex = GetMemoryType(a_Device, t_MemoryRequirements.memoryTypeBits,
	                                               a_Properties);

	if (vkAllocateMemory(a_Device.GetLogicalDevice(), &t_AllocateInfo, nullptr, &m_Memory))
	{
		throw std::runtime_error("Error: Could not allocate Vertex VertexBuffer Memory!");
	}
}

void Buffer::FillBuffer(VkDeviceSize a_BufferSize, const VkDevice& a_LogicalDevice, void* a_Data)
{
	vkMapMemory(a_LogicalDevice, m_Memory, 0, a_BufferSize, 0, &m_Data);
	memcpy(m_Data, a_Data, static_cast<size_t>(a_BufferSize));
	vkUnmapMemory(a_LogicalDevice, m_Memory);
}
