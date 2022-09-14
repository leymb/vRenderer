#pragma once
#include "vulkan/vulkan_core.h"

#include <vector>

inline VkPipelineDynamicStateCreateInfo GenDynamicStateCreateInfo(const std::vector<VkDynamicState>& a_DynamicStates)
{
	VkPipelineDynamicStateCreateInfo t_DynamicStateCreateInfo = {};

	t_DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	t_DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(a_DynamicStates.size());
	t_DynamicStateCreateInfo.pDynamicStates = a_DynamicStates.data();

	return t_DynamicStateCreateInfo;
}

inline VkPipelineVertexInputStateCreateInfo GenVertexInputStateCreateInfo()
{
	// this allows for defining the format of vertex data passed to the vertex shader
	// as right now the vertex data is hard-coded in the shader, this is left
	// intentionally empty
	VkPipelineVertexInputStateCreateInfo t_VertexInputStateCreateInfo = {};
	t_VertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	t_VertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
	t_VertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
	t_VertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	t_VertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;

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
	t_RasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

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

inline VkPipelineLayoutCreateInfo GenPipelineCreateInfo()
{
	VkPipelineLayoutCreateInfo t_PipelineLayoutCreateInfo = {};

	t_PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	t_PipelineLayoutCreateInfo.setLayoutCount = 0;
	t_PipelineLayoutCreateInfo.pSetLayouts = nullptr;
	t_PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	t_PipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	return t_PipelineLayoutCreateInfo;
}