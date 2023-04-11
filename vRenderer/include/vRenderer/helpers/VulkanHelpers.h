#pragma once
#include "vulkan/vulkan_core.h"

#include <vector>

#include "vRenderer/helper_structs/Vertex.h"

inline VkPipelineDynamicStateCreateInfo GenDynamicStateCreateInfo(const std::vector<VkDynamicState>& a_DynamicStates)
{
	VkPipelineDynamicStateCreateInfo t_DynamicStateCreateInfo = {};

	t_DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	t_DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(a_DynamicStates.size());
	t_DynamicStateCreateInfo.pDynamicStates = a_DynamicStates.data();

	return t_DynamicStateCreateInfo;
}

inline VkPipelineVertexInputStateCreateInfo GenVertexInputStateCreateInfo(
	VkVertexInputBindingDescription& a_BindingDesc, std::array<VkVertexInputAttributeDescription, 3>& a_AttributeDesc)
{
	VkPipelineVertexInputStateCreateInfo t_VertexInputStateCreateInfo = {};
	t_VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	t_VertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	t_VertexInputStateCreateInfo.pVertexBindingDescriptions = &a_BindingDesc;

	t_VertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(a_AttributeDesc.size());
	t_VertexInputStateCreateInfo.pVertexAttributeDescriptions = a_AttributeDesc.data();

	return t_VertexInputStateCreateInfo;
}

inline VkPipelineInputAssemblyStateCreateInfo GenInputAssemblyStateCreateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo t_InputAssemblyStateCreateInfo = {};
	t_InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	t_InputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	t_InputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	return t_InputAssemblyStateCreateInfo;
}

inline VkViewport GenViewportData(const VkExtent2D& a_SwapChainExtent)
{
	VkViewport t_Viewport = {};
	t_Viewport.x = 0.0f;
	t_Viewport.y = 0.0f;
	t_Viewport.width = static_cast<float>(a_SwapChainExtent.width);
	t_Viewport.height = static_cast<float>(a_SwapChainExtent.height);
	t_Viewport.minDepth = 0.0f;
	t_Viewport.maxDepth = 1.0f;

	return t_Viewport;
}

// TODO hand a list of viewports and scissor rects to this function
inline VkPipelineViewportStateCreateInfo GenViewportStateCreateInfo(const int a_ViewportCount, const VkViewport& a_Viewport,
                                                                    const int a_ScissorCount, const VkRect2D a_ScissorRect)
{
	VkPipelineViewportStateCreateInfo t_ViewportCreateInfo = {};

	t_ViewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	t_ViewportCreateInfo.viewportCount = a_ViewportCount;
	t_ViewportCreateInfo.pViewports = &a_Viewport;
	t_ViewportCreateInfo.scissorCount = a_ScissorCount;
	t_ViewportCreateInfo.pScissors = &a_ScissorRect;

	return t_ViewportCreateInfo;
}

inline VkPipelineRasterizationStateCreateInfo GenRasterizationStateCreateInfo()
{
	VkPipelineRasterizationStateCreateInfo t_RasterizationStateCreateInfo = {};

	t_RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	t_RasterizationStateCreateInfo.depthClampEnable = VK_FALSE;

	t_RasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;

	// define how fragments are generated for geometry
	t_RasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;

	t_RasterizationStateCreateInfo.lineWidth = 1.0f;

	// set cull mode
	t_RasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;

	// set vertex order for faces
	t_RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	t_RasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	t_RasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
	t_RasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	t_RasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

	return t_RasterizationStateCreateInfo;
}

inline VkPipelineMultisampleStateCreateInfo GenMultisamplingStateCreateInfo()
{
	VkPipelineMultisampleStateCreateInfo t_MultisampleState = {};

	t_MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	t_MultisampleState.sampleShadingEnable = VK_FALSE;
	t_MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	t_MultisampleState.minSampleShading = 1.0f;
	t_MultisampleState.pSampleMask = nullptr;
	t_MultisampleState.alphaToCoverageEnable = VK_FALSE;
	t_MultisampleState.alphaToOneEnable = VK_FALSE;

	return t_MultisampleState;
}

inline VkPipelineColorBlendAttachmentState GenColorBlendAttachStateCreateInfo()
{
	VkPipelineColorBlendAttachmentState t_ColorBlend = {};
	t_ColorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	t_ColorBlend.blendEnable = VK_FALSE;
	t_ColorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	t_ColorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	t_ColorBlend.colorBlendOp = VK_BLEND_OP_ADD;
	t_ColorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	t_ColorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	t_ColorBlend.alphaBlendOp = VK_BLEND_OP_ADD;

	return t_ColorBlend;
}

inline VkPipelineColorBlendStateCreateInfo GenColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState& a_ColorBlendAttachmentState)
{
	VkPipelineColorBlendStateCreateInfo t_ColorBlendStateCreateInfo = {};

	t_ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	t_ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	t_ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	t_ColorBlendStateCreateInfo.attachmentCount = 1;
	t_ColorBlendStateCreateInfo.pAttachments = &a_ColorBlendAttachmentState;
	t_ColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
	t_ColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
	t_ColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
	t_ColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

	return t_ColorBlendStateCreateInfo;
}

inline VkPipelineLayoutCreateInfo GenPipelineCreateInfo(const int a_LayoutCount, const VkDescriptorSetLayout* a_Layout)
{
	VkPipelineLayoutCreateInfo t_PipelineLayoutCreateInfo = {};

	t_PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	t_PipelineLayoutCreateInfo.setLayoutCount = a_LayoutCount;
	t_PipelineLayoutCreateInfo.pSetLayouts = a_Layout;
	t_PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	t_PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	return t_PipelineLayoutCreateInfo;
}

inline void DestroyFrameBuffers(std::vector<VkFramebuffer>& a_FramebufferVector, const VkDevice& a_Device)
{
	for (VkFramebuffer t_Buffer : a_FramebufferVector)
	{
		vkDestroyFramebuffer(a_Device, t_Buffer, nullptr);
	}
}

/// <summary>
/// 	Checks whether any provided Queue supports the operations required by this application.
/// </summary>
/// <param name="a_QueueFamilyProperty">	The properties of the queue family for which to check
/// 										the suitability.</param>
/// <returns>
/// 	True if the Queue family supports the required operations, false if it does not.
/// </returns>
inline bool CheckQueueFamilySupportedOperations(VkQueueFamilyProperties a_QueueFamilyProperty)
{
	// Check for supported operations.
	if (a_QueueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
	{
		return true;
	}

	return false;
}

inline bool CheckQueueFamilySupportsPresentation(VkPhysicalDevice a_Device, uint32_t a_QueueFamilyIndex, VkSurfaceKHR a_Surface)
{
	// check whether the queue family supports presenting to the Window Surface
	VkBool32 t_SupportsPresenting = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(a_Device, a_QueueFamilyIndex, a_Surface, &t_SupportsPresenting);

	if (t_SupportsPresenting)
	{
		return true;
	}

	return false;
}

/// <summary>
/// 	Gets all queue families the device supports. Also checks whether any of them supports the
/// 	operations required by this application.
/// </summary>
/// <param name="a_Device">	The device for which to check the supported queue families.</param>
/// <returns>	The SupportedQueueFamilies. </returns>
inline SupportedQueueFamilies CheckSupportedQueueFamilies(const VkPhysicalDevice a_Device, VkSurfaceKHR a_Surface)
{
	SupportedQueueFamilies t_QueueFamilies;

	// get the number of supported queue families
	uint32_t t_NumQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(a_Device, &t_NumQueueFamilies, nullptr);

	// retrieve the queue families' properties and store them
	std::vector<VkQueueFamilyProperties> t_QueueFamilyProperties(t_NumQueueFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(a_Device, &t_NumQueueFamilies, t_QueueFamilyProperties.data());

	// loop over the queues found and check whether any of them is suitable
	for (unsigned int i = 0; i < t_NumQueueFamilies; i++)
	{
		if (CheckQueueFamilySupportedOperations(t_QueueFamilyProperties[i]))
		{
			t_QueueFamilies.m_GraphicsFamily = i;
		}

		if (CheckQueueFamilySupportsPresentation(a_Device, i, a_Surface))
		{
			t_QueueFamilies.m_PresentFamily = i;
		}

		if (t_QueueFamilies.IsComplete())
		{
			break;
		}
	}

	return  t_QueueFamilies;
}

/// <summary>	Begins single time command. </summary>
/// <param name="a_CommandPool">  	[in,out] The command pool.</param>
/// <param name="a_LogicalDevice">	The logical device.</param>
/// <returns>	A VkCommandBuffer. </returns>

inline VkCommandBuffer BeginSingleTimeCommand(VkCommandPool& a_CommandPool, const VkDevice& a_LogicalDevice)
{
	// create temporary command buffer
	VkCommandBufferAllocateInfo t_BufferAllocateInfo = {};
	t_BufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	t_BufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	t_BufferAllocateInfo.commandPool = a_CommandPool;
	t_BufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer t_CommandBuffer;
	vkAllocateCommandBuffers(a_LogicalDevice, &t_BufferAllocateInfo, &t_CommandBuffer);

	// begin recording command buffer
	VkCommandBufferBeginInfo t_BufferBeginInfo = {};
	t_BufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	t_BufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(t_CommandBuffer, &t_BufferBeginInfo);

	return  t_CommandBuffer;
}

/// <summary>	Ends single time commands. </summary>
/// <param name="a_CommandBuffer">	Buffer for command data.</param>
/// <param name="a_GraphicsQueue">	Queue of graphics.</param>
/// <param name="a_LogicalDevice">	The logical device.</param>
/// <param name="a_CommandPool">  	The command pool.</param>

inline void EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer, const VkQueue& a_GraphicsQueue,
                                  const VkDevice& a_LogicalDevice, const VkCommandPool& a_CommandPool)
{
	// end recording command buffer
	vkEndCommandBuffer(a_CommandBuffer);

	VkSubmitInfo t_SubmitInfo = {};
	t_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	t_SubmitInfo.commandBufferCount = 1;
	t_SubmitInfo.pCommandBuffers = &a_CommandBuffer;

	vkQueueSubmit(a_GraphicsQueue, 1, &t_SubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(a_GraphicsQueue);

	// cleanup 
	vkFreeCommandBuffers(a_LogicalDevice,a_CommandPool, 1, &a_CommandBuffer);
}

/// <summary>	Searches for the first supported format. </summary>
/// <exception cref="std::runtime_error">	Raised when no supported format could be found.</exception>
/// <param name="a_CandidateList"> 	List of candidates.</param>
/// <param name="a_Tiling">		   	The tiling mode.</param>
/// <param name="a_FeatureFlags">  	The feature flags.</param>
/// <param name="a_PhysicalDevice">	The physical device.</param>
/// <returns>	The found supported format. </returns>

inline VkFormat FindSupportedFormat(const std::vector<VkFormat>& a_CandidateList, VkImageTiling a_Tiling,
                                VkFormatFeatureFlags a_FeatureFlags, VkPhysicalDevice a_PhysicalDevice)
{
	for (VkFormat t_Format : a_CandidateList)
	{
		VkFormatProperties t_FormatProperties;
		vkGetPhysicalDeviceFormatProperties(a_PhysicalDevice, t_Format, &t_FormatProperties);

		// if format is optimal for linear image tiling
		if (a_Tiling == VK_IMAGE_TILING_LINEAR && (t_FormatProperties.linearTilingFeatures & a_FeatureFlags) ==
			a_FeatureFlags)
		{
			return t_Format;
		}

		// if format is optimal for optimal image tiling
		if (a_Tiling == VK_IMAGE_TILING_OPTIMAL && (t_FormatProperties.optimalTilingFeatures & a_FeatureFlags) ==
			a_FeatureFlags)
		{
			return t_Format;
		}
	}

	// if no supported format could be found, throw an exception
	throw std::runtime_error("Error! Could not find supported format!");
}

inline VkFormat FindDepthFormat(VkPhysicalDevice a_PhysicalDevice)
{
	return FindSupportedFormat(
		{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
		a_PhysicalDevice
	);
}

inline bool HasStencilComponent(VkFormat a_Format)
{
	return a_Format == VK_FORMAT_D32_SFLOAT_S8_UINT || a_Format == VK_FORMAT_D24_UNORM_S8_UINT;
}