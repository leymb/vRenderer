#include "pch.h"
#include "vRenderer/vRenderer.h"
#include "vRenderer/helper_structs/RenderingHelpers.h"
#include "vRenderer/helpers/helpers.h"
#include "vRenderer/helpers/VulkanHelpers.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "vRenderer/helper_structs/Vertex.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "vRenderer/helper_structs/UniformBufferObject.h"

// test vertices and indices 
static std::vector<Vertex> s_quad_vertices = {
	{{-0.5f, -0.5f}, {1.f, 0.f, 0.f}},
    {{0.5f, -0.5f}, {0.f, 1.f, 0.f}},
    {{0.5f, 0.5f}, {0.f, 0.f, 1.f}},
	{{-0.5f, 0.5f}, {1.f, 1.f, 1.f}}
};

static std::vector<uint16_t> s_quad_indices = {
	0, 1, 2,
	2, 3, 0
};

VRenderer::VRenderer(): m_Window(nullptr)
{
}

bool VRenderer::Init(const int a_WindowWidth, const int a_WindowHeight,
                     const std::vector<const char*>& a_EnabledValidationLayers,
                     const std::vector<const char*>& a_RequestedDeviceExtensions)
{
	m_EnabledValidationLayers = a_EnabledValidationLayers;
	m_RequestedDeviceExtensions = a_RequestedDeviceExtensions;

	EnableValidation();

	InitGlfw(a_WindowWidth, a_WindowHeight);

	InitVulkan();

	return true;
}

bool VRenderer::Terminate()
{
	// wait for asynchronous processes to finish
	vkDeviceWaitIdle(m_Device.GetLogicalDevice());

	DestroySyncObjects();
	vkDestroyCommandPool(m_Device.GetLogicalDevice(), m_CommandPool, nullptr);
	DestroyFrameBuffers(m_Framebuffers, m_Device.GetLogicalDevice());
	vkDestroyPipeline(m_Device.GetLogicalDevice(), m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_Device.GetLogicalDevice(), m_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_Device.GetLogicalDevice(), m_MainRenderPass, nullptr);
	m_SwapChain.DestroyImageViews(m_Device.GetLogicalDevice());
	vkDestroySwapchainKHR(m_Device.GetLogicalDevice(), m_SwapChain.GetSwapChain(), nullptr);

	for(size_t i = 0; i < m_UniformBuffers.size(); i++)
	{
		m_UniformBuffers[i].DestroyBuffer(m_Device.GetLogicalDevice());
	}

	vkDestroyDescriptorPool(m_Device.GetLogicalDevice(), m_DescriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(m_Device.GetLogicalDevice(), m_DescriptorSetLayout, nullptr);

	m_VertexBuffer.DestroyBuffer(m_Device.GetLogicalDevice());
	m_IndexBuffer.DestroyBuffer(m_Device.GetLogicalDevice());

	vkDestroyDevice(m_Device.GetLogicalDevice(), nullptr);
	vkDestroySurfaceKHR(m_VInstance, m_WindowSurface, nullptr);
	vkDestroyInstance(m_VInstance, nullptr);
	glfwDestroyWindow(m_Window);
	return true;
}

void VRenderer::Render()
{
	glfwPollEvents();

	// wait for previous frame
	vkWaitForFences(m_Device.GetLogicalDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

	// reset fences
	vkResetFences(m_Device.GetLogicalDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

	// acquire image from swap chain
	uint32_t t_ImageIndex;
	vkAcquireNextImageKHR(m_Device.GetLogicalDevice(), m_SwapChain.GetSwapChain(), UINT64_MAX, m_ImageAcquiredSemaphores[m_CurrentFrame],
	                      VK_NULL_HANDLE, &t_ImageIndex);

	// update uniform buffers
	UpdateUniformBuffers(m_CurrentFrame);

	// record command buffer
	vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
	RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], t_ImageIndex);

	//submit command buffer
	VkSubmitInfo t_CommandBufferSubmitInfo = {};
	t_CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore t_WaitSemaphores[] = {m_ImageAcquiredSemaphores[m_CurrentFrame]};
	VkPipelineStageFlags t_WaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	t_CommandBufferSubmitInfo.waitSemaphoreCount = 1;
	t_CommandBufferSubmitInfo.pWaitSemaphores = t_WaitSemaphores;
	t_CommandBufferSubmitInfo.pWaitDstStageMask = t_WaitStages;

	t_CommandBufferSubmitInfo.commandBufferCount = 1;
	t_CommandBufferSubmitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

	VkSemaphore t_SignalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
	t_CommandBufferSubmitInfo.signalSemaphoreCount = 1;
	t_CommandBufferSubmitInfo.pSignalSemaphores = t_SignalSemaphores;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &t_CommandBufferSubmitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not submit Draw Command VertexBuffer!");
	}

	VkPresentInfoKHR t_PresentInfo = {};
	t_PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	t_PresentInfo.waitSemaphoreCount = 1;
	t_PresentInfo.pWaitSemaphores = t_SignalSemaphores;

	VkSwapchainKHR t_SwapChains[] = {m_SwapChain.GetSwapChain()};
	t_PresentInfo.swapchainCount = 1;
	t_PresentInfo.pSwapchains = t_SwapChains;
	t_PresentInfo.pImageIndices = &t_ImageIndex;
	t_PresentInfo.pResults = nullptr;

	vkQueuePresentKHR(m_PresentQueue, &t_PresentInfo);

	// advance frame (and loop it around after m_MaxInFlightFrames)
	m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxInFlightFrames;
}

bool VRenderer::ShouldTerminate() const
{
	return glfwWindowShouldClose(m_Window);
}

void VRenderer::InitVulkan()
{
	CreateInstance();
	CreateWindowSurface();

	m_Device.ChoosePhysicalDevice(m_VInstance, m_WindowSurface, m_RequestedDeviceExtensions);
	m_Device.CreateLogicalDevice(m_WindowSurface, m_GraphicsQueue, m_PresentQueue, m_RequestedDeviceExtensions,
	                             m_EnabledValidationLayers);

	m_SwapChain.Create(m_Device, m_WindowSurface, m_Window);
	m_SwapChain.CreateImageViews(m_Device.GetLogicalDevice());
	CreateRenderPass();
	m_DescriptorSetLayout = UniformBuffer::CreateDescriptorSetLayout(m_Device.GetLogicalDevice());
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();

	m_VertexBuffer.CreateVertexBuffer(s_quad_vertices, m_Device, m_GraphicsQueue, m_CommandPool);
	m_IndexBuffer.CreateIndexBuffer(s_quad_indices, m_Device, m_GraphicsQueue, m_CommandPool);
	CreateUniformBuffers();
	m_DescriptorPool = CreateDescriptorPool(m_MaxInFlightFrames, m_Device.GetLogicalDevice());
	CreateDescriptorSets(m_MaxInFlightFrames, m_Device.GetLogicalDevice(), m_DescriptorSetLayout, m_DescriptorPool,
	                     m_DescriptorSets);
	
	CreateCommandBuffers();
	CreateSyncObjects();
}

/// <summary>	Initializes GLFW and creates a GLFWwindow. </summary>
/// <param name="a_WindowWidth"> 	Width of the window.</param>
/// <param name="a_WindowHeight">	Height of the window.</param>

void VRenderer::InitGlfw(const int a_WindowWidth, const int a_WindowHeight)
{
	glfwInit();

	// tell GLFW not to create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// tell GFLW to not make the window resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(a_WindowWidth, a_WindowHeight, "vRenderer", nullptr, nullptr);
}

/// <summary>	Creates a Vulkan Instance and stores its handle in m_VInstance. </summary>
/// <exception cref="std::runtime_error">	Raised when the instance could not be created.</exception>
/// <returns>	True if it succeeds, false if it fails. </returns>

bool VRenderer::CreateInstance()
{
	bool t_EnableValidation = false;
#ifdef _DEBUG
	EnableValidation();
	t_EnableValidation = true;
#endif

	InstanceCreateData t_InstanceCreateData = {};
	GenInstanceCreateData(t_EnableValidation, t_InstanceCreateData);

	// create Vulkan instance & store its handle
	const VkResult t_Result = vkCreateInstance( &t_InstanceCreateData.m_CreateInfo, nullptr, &m_VInstance);

	if (t_Result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create instance!");
	}

	return true;
}

/// <summary>	Checks whether the provided list of extensions is supported. </summary>
/// <exception cref="std::runtime_error">	Raised when a requested extension is not available.</exception>
/// <param name="a_ExtensionCount">	Number of extensions.</param>
/// <param name="a_Extensions">	   	The extensions.</param>
/// <returns>	True if it succeeds, false if it fails. </returns>

bool VRenderer::CheckExtensionSupport(const uint32_t a_ExtensionCount, const char** a_Extensions)
{
	// find how many extensions are supported
	uint32_t t_ExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &t_ExtensionCount, nullptr);

	// get supported extensions
	std::vector<VkExtensionProperties>t_Extensions(t_ExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &t_ExtensionCount, t_Extensions.data());
#ifdef _DEBUG
	std::cout << "Extensions:\n" << std::endl;

	for (const auto& t_Extension : t_Extensions)
	{
		std::cout << '\t' << t_Extension.extensionName << '\n' << std::endl;
	}

#endif

	for (unsigned int i = 0; i < a_ExtensionCount; i++)
	{
		bool t_ExtensionFound = false;
		for (const auto t_Extension : t_Extensions)
		{
			if(strcmp(a_Extensions[i], t_Extension.extensionName)){
                  t_ExtensionFound = true;          
			}
		}

		if (!t_ExtensionFound)
		{
			std::string t_Exception = "Missing Vulkan Extension! Missing extension: ";
			t_Exception.append(a_Extensions[i]);
			throw std::runtime_error(t_Exception);
		}
	}

#ifdef _DEBUG
		std::cout << "All " << a_ExtensionCount << " extensions are supported." << std::endl;
#endif
	return true;
}

void VRenderer::GenInstanceCreateData(const bool a_ValidationEnabled, InstanceCreateData& a_InstanceCreateData) const
{
	// specify application information
	VkApplicationInfo& t_ApplicationInfo = a_InstanceCreateData.m_ApplicationInfo;
	t_ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	t_ApplicationInfo.pApplicationName = "Triangle Project";
	t_ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
	t_ApplicationInfo.pEngineName = "No Engine";
	t_ApplicationInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
	t_ApplicationInfo.apiVersion = VK_API_VERSION_1_0;
	t_ApplicationInfo.pNext = VK_NULL_HANDLE;

	// make instance create info from application info
	VkInstanceCreateInfo& t_InstanceDesc = a_InstanceCreateData.m_CreateInfo;
	t_InstanceDesc.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	t_InstanceDesc.pApplicationInfo = &t_ApplicationInfo;

	// get the extensions GLFW needs and pass it to the instance
	uint32_t t_GlfwExtensionCount = 0;
	const char** t_GlfwExtensions = glfwGetRequiredInstanceExtensions(&t_GlfwExtensionCount);

	// check whether the extensions required by GLFW are available
	CheckExtensionSupport(t_GlfwExtensionCount, t_GlfwExtensions);

	t_InstanceDesc.enabledExtensionCount = t_GlfwExtensionCount;
	t_InstanceDesc.ppEnabledExtensionNames = t_GlfwExtensions;

	// if validation is enabled, add validation information to instance create info
	if (a_ValidationEnabled)
	{
		t_InstanceDesc.enabledLayerCount = static_cast<uint32_t>(m_EnabledValidationLayers.size());
		t_InstanceDesc.ppEnabledLayerNames = m_EnabledValidationLayers.data();
	}
	else
	{
		t_InstanceDesc.enabledLayerCount = 0;	
	}
}


/// <summary>	Enables validation layers. </summary>

void VRenderer::EnableValidation() const
{
	// throw an exception if any of the requested validation layers are not available in the system
	CheckSupportedValidationLayers(m_EnabledValidationLayers);
}

/// <summary>	Checks whether the requested valiadtion layers are supported. </summary>
/// <exception cref="std::runtime_error">	Raised when requested validation layer is not
/// 										available.</exception>
/// <param name="a_RequestedValidationLayers">	The requested validation layers.</param>
/// <returns>
/// 	True if the requested validation layers are supported, else throws a runtime exception.
/// </returns>

bool VRenderer::CheckSupportedValidationLayers(const std::vector<const char*>& a_RequestedValidationLayers)
{
	// get the amount of supported layers
	uint32_t t_LayerCount = 0;
	vkEnumerateInstanceLayerProperties(&t_LayerCount, nullptr);

	// get the supported layers
	std::vector<VkLayerProperties> t_LayersPresent(t_LayerCount);
	vkEnumerateInstanceLayerProperties(&t_LayerCount, t_LayersPresent.data());

	// for every requested layer, check if a layer with the same name exists in
	// the list of layers present in the system
	for (const char* t_RequestedLayerName : a_RequestedValidationLayers)
	{
		bool t_LayerFound = false;

		for (const auto& t_AvailableLayer : t_LayersPresent)
		{
			if (strcmp(t_RequestedLayerName, t_AvailableLayer.layerName) == 0)
			{
				t_LayerFound = true;
			}
		}
		if (!t_LayerFound)
		{
			std::string t_ErrorMessage = "\nRequested Validation Layer: ";
			t_ErrorMessage.append(t_RequestedLayerName);
			t_ErrorMessage.append(" is not available!\n");

			throw std::runtime_error(t_ErrorMessage);
			// return false;
		}
	}

	return true;
}

void VRenderer::CreateWindowSurface()
{
	if (glfwCreateWindowSurface(m_VInstance, m_Window, nullptr, &m_WindowSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Window surface could not be created!");
	}
}

/// <summary>
/// 	Creates the graphics pipeline and the required fixed stage functions, render passes,
/// 	viewport and scissor descriptions.
/// </summary>
/// <exception cref="std::runtime_error">	Raised when pipeline layout could not be created.</exception>

void VRenderer::CreateGraphicsPipeline()
{
	// TODO separate this into several functions

	// generate Shader modules
	const auto t_VertexShaderByteCode = ReadFile("../vRenderer/assets/shaders/compiled/vertex_shader.spv");
	const auto t_FragmentShaderByteCode = ReadFile("../vRenderer/assets/shaders/compiled/fragment_shader.spv");

	const VkShaderModule t_VertexShader = GenShaderModule(t_VertexShaderByteCode);
	const VkShaderModule t_FragmentShader = GenShaderModule(t_FragmentShaderByteCode);

	// generate vertex shader stage
	VkPipelineShaderStageCreateInfo t_VertShaderStageInfo = {};
	t_VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	t_VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	// specify where the shader code is located and what the entry point is
	t_VertShaderStageInfo.module = t_VertexShader;
	t_VertShaderStageInfo.pName = "main";

	// generate fragment shader stage
	VkPipelineShaderStageCreateInfo t_FragShaderStageInfo = {};
	t_FragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	t_FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	// specify where the shader code is located and what the entry point is
	t_FragShaderStageInfo.module = t_FragmentShader;
	t_FragShaderStageInfo.pName = "main";


	// store shader stage create infos
	VkPipelineShaderStageCreateInfo t_ShaderStageCreateInfos[] = {t_VertShaderStageInfo, t_FragShaderStageInfo};

	// set dynamic states
	std::vector<VkDynamicState> t_DynStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo t_DynamicStateCreateInfo = GenDynamicStateCreateInfo(t_DynStates);

	// create VertexInputStateCreateInfo
	VkVertexInputBindingDescription t_BindingDesc = Vertex::GenInputBindingDesc();
	std::array<VkVertexInputAttributeDescription, 2> t_AttributeDesc = Vertex::GenInputAttributeDesc();
	VkPipelineVertexInputStateCreateInfo t_VertexInputStateCreateInfo = GenVertexInputStateCreateInfo(t_BindingDesc, t_AttributeDesc);

	// generate Input Assembly stage
	VkPipelineInputAssemblyStateCreateInfo t_InputAssemblyStateCreateInfo = GenInputAssemblyStateCreateInfo();

	// generate Viewport
	VkViewport t_Viewport = GenViewportData(m_SwapChain.GetExtent());

	// generate scissor rectangle
	VkRect2D t_ScissorRect = {};
	t_ScissorRect.offset = {0,0};
	t_ScissorRect.extent = m_SwapChain.GetExtent();

	// generate Viewport State 
	VkPipelineViewportStateCreateInfo t_ViewportState = GenViewportStateCreateInfo(1, t_Viewport, 1, t_ScissorRect);

	// generate Rasterizer State
	VkPipelineRasterizationStateCreateInfo t_RasterizationStateCreateInfo = GenRasterizationStateCreateInfo();

	// generate Multisampling State Create Info
	VkPipelineMultisampleStateCreateInfo t_MultisampleState = GenMultisamplingStateCreateInfo();


	// generate ColorBlendAttachementState CreateInfo
	VkPipelineColorBlendAttachmentState t_ColorBlendAttachementState = GenColorBlendAttachStateCreateInfo();

	// generate ColorBlendState CreateInfo
	VkPipelineColorBlendStateCreateInfo t_ColorBlendStateCreateInfo = GenColorBlendStateCreateInfo(t_ColorBlendAttachementState);


	// generate Pipeline Layout
	VkPipelineLayoutCreateInfo t_PipelineLayoutCreateInfo = GenPipelineCreateInfo(1, &m_DescriptorSetLayout);

	if (vkCreatePipelineLayout(m_Device.GetLogicalDevice(), &t_PipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Pipeline Layout!");
	}


	// make Graphics Pipeline
	VkGraphicsPipelineCreateInfo t_PipelineCreateInfo = {};
	t_PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	t_PipelineCreateInfo.stageCount = 2;
	t_PipelineCreateInfo.pStages = t_ShaderStageCreateInfos;

	// add all of the previously created pipeline state descriptions (fixed function stages)
	t_PipelineCreateInfo.pVertexInputState = &t_VertexInputStateCreateInfo;
	t_PipelineCreateInfo.pInputAssemblyState = &t_InputAssemblyStateCreateInfo;
	t_PipelineCreateInfo.pViewportState = &t_ViewportState;
	t_PipelineCreateInfo.pRasterizationState = &t_RasterizationStateCreateInfo;
	t_PipelineCreateInfo.pMultisampleState = &t_MultisampleState;
	t_PipelineCreateInfo.pDepthStencilState = nullptr;
	t_PipelineCreateInfo.pColorBlendState = &t_ColorBlendStateCreateInfo;
	t_PipelineCreateInfo.pDynamicState = &t_DynamicStateCreateInfo;

	// pass handle to pipeline layout
	t_PipelineCreateInfo.layout = m_PipelineLayout;

	// reference render pass
	t_PipelineCreateInfo.renderPass = m_MainRenderPass;
	t_PipelineCreateInfo.subpass = 0;

	t_PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	t_PipelineCreateInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(m_Device.GetLogicalDevice(), VK_NULL_HANDLE, 1, &t_PipelineCreateInfo, nullptr, &m_GraphicsPipeline) 
		!= VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create Graphics Pipeline!");
	}


	//destroy Shader modules as they are no longer needed
	vkDestroyShaderModule(m_Device.GetLogicalDevice(), t_VertexShader, nullptr);
	vkDestroyShaderModule(m_Device.GetLogicalDevice(), t_FragmentShader, nullptr);
}

/// <summary>	Generates a shader module from the provided vector of bytecode. </summary>
/// <exception cref="std::runtime_error">	Raised when a runtime error condition occurs.</exception>
/// <param name="a_CodeData">	The shader bytecode loaded from a compiled .spv file.</param>
/// <returns>	The shader module. </returns>

VkShaderModule VRenderer::GenShaderModule(const std::vector<char>& a_CodeData)
{
	VkShaderModuleCreateInfo t_ShaderModuleCreateInfo = {};
	t_ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	t_ShaderModuleCreateInfo.codeSize = a_CodeData.size();
	t_ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(a_CodeData.data());

	VkShaderModule t_Module;
	if (vkCreateShaderModule(m_Device.GetLogicalDevice(), &t_ShaderModuleCreateInfo, nullptr, &t_Module) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Shader Module!");
	}

	return t_Module;
}

/// <summary>	Creates a render pass. </summary>
/// <exception cref="std::runtime_error">	Raised when render pass can not be created.</exception>

void VRenderer::CreateRenderPass()
{
	// color buffer attachment
	VkAttachmentDescription t_ColorAttachement = {};
	t_ColorAttachement.format = m_SwapChain.GetFormat();

	// no multisampling, so keep this to 1
	t_ColorAttachement.samples = VK_SAMPLE_COUNT_1_BIT;

	// clear the framebuffer before drawing a new frame
	t_ColorAttachement.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

	// store rendered contents
	t_ColorAttachement.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	t_ColorAttachement.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	t_ColorAttachement.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	// specify the layout of the image before and after the render pass finishes
	t_ColorAttachement.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	t_ColorAttachement.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	// attachment references
	VkAttachmentReference t_ColorAttachmentReference = {};
	t_ColorAttachmentReference.attachment = 0;
	t_ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	// sub-passes
	VkSubpassDescription t_SubpassDescription = {};

	// describe this as a Graphics pass (and not a Compute pass)
	t_SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	t_SubpassDescription.colorAttachmentCount = 1;
	t_SubpassDescription.pColorAttachments = &t_ColorAttachmentReference;

	// handle sub-pass dependencies
	VkSubpassDependency t_SubpassDependency = {};
	t_SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	t_SubpassDependency.dstSubpass = 0;
	t_SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	t_SubpassDependency.srcAccessMask = 0;
	t_SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	t_SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// create render pass
	VkRenderPassCreateInfo t_RenderPassCreateInfo = {};
	t_RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	t_RenderPassCreateInfo.attachmentCount = 1;
	t_RenderPassCreateInfo.pAttachments = &t_ColorAttachement;
	t_RenderPassCreateInfo.subpassCount = 1;
	t_RenderPassCreateInfo.pSubpasses = &t_SubpassDescription;
	t_RenderPassCreateInfo.dependencyCount = 1;
	t_RenderPassCreateInfo.pDependencies = &t_SubpassDependency;

	if (vkCreateRenderPass(m_Device.GetLogicalDevice(), &t_RenderPassCreateInfo, nullptr, &m_MainRenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Renderpass!");
	}
}

void VRenderer::CreateFrameBuffers()
{
	// resize Framebuffers vector to be able to hold one frame buffer per swap chain image
	m_Framebuffers.resize(m_SwapChain.GetImageViews().size());

	std::vector<VkImageView> t_SwapChainImageViews = m_SwapChain.GetImageViews();
	const VkExtent2D t_SwapChainExtent = m_SwapChain.GetExtent();

	// iterate over the SwapChainImageViews vector and create a frame buffer per image
	for (size_t i = 0; i < t_SwapChainImageViews.size(); i++)
	{
		const VkImageView t_Attachments[] = 
		{
			t_SwapChainImageViews[i]
		};

		VkFramebufferCreateInfo t_BufferCreateInfo = {};
		t_BufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		t_BufferCreateInfo.renderPass = m_MainRenderPass;
		t_BufferCreateInfo.attachmentCount = 1;
		t_BufferCreateInfo.pAttachments = t_Attachments;
		t_BufferCreateInfo.width = t_SwapChainExtent.width;
		t_BufferCreateInfo.height = t_SwapChainExtent.height;
		t_BufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(m_Device.GetLogicalDevice(), &t_BufferCreateInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not create Framebuffer!");
		}
	}
}

void VRenderer::CreateCommandPool()
{
	const SupportedQueueFamilies t_QueueFamilyIndices = CheckSupportedQueueFamilies(m_Device.GetPhysicalDevice(), m_WindowSurface);

	// TODO move this into its own function in its own class
	VkCommandPoolCreateInfo t_CommandPoolCreateInfo = {};
	t_CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

	// allow command buffers to be rerecorded individually
	t_CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	t_CommandPoolCreateInfo.queueFamilyIndex = t_QueueFamilyIndices.m_GraphicsFamily.value();

	// create command pool
	if (vkCreateCommandPool(m_Device.GetLogicalDevice(), &t_CommandPoolCreateInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Command Pool!");
	}
}

void VRenderer::CreateCommandBuffers()
{
	// allocate Command Buffers
	m_CommandBuffers.resize(m_MaxInFlightFrames);

	VkCommandBufferAllocateInfo t_CommandBufferAllocateInfo = {};
	t_CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	t_CommandBufferAllocateInfo.commandPool = m_CommandPool;
	t_CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	t_CommandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.GetLogicalDevice(), &t_CommandBufferAllocateInfo, m_CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not allocate Command VertexBuffer!");
	}
}

/// <summary>	Records commands to a command buffer. </summary>
/// <exception cref="std::runtime_error">	Raised when a runtime error condition occurs.</exception>
/// <param name="a_CommandBuffer">	Command VertexBuffer.</param>
/// <param name="a_ImageIndex">   	Zero-based index of the image.</param>

void VRenderer::RecordCommandBuffer(VkCommandBuffer a_CommandBuffer, uint32_t a_ImageIndex)
{
	// record commands to the command buffer
	VkCommandBufferBeginInfo t_CommandBufferBeginInfo = {};

	t_CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	t_CommandBufferBeginInfo.flags = 0;
	t_CommandBufferBeginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &t_CommandBufferBeginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not begin recording the Command VertexBuffer!");
	}


	// start render pass
	VkRenderPassBeginInfo t_RenderPassBeginInfo = {};
	t_RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	t_RenderPassBeginInfo.renderPass = m_MainRenderPass;
	t_RenderPassBeginInfo.framebuffer = m_Framebuffers[a_ImageIndex];
	t_RenderPassBeginInfo.renderArea.offset = {0,0};
	t_RenderPassBeginInfo.renderArea.extent = m_SwapChain.GetExtent();

	const VkClearValue t_ClearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	t_RenderPassBeginInfo.clearValueCount = 1;
	t_RenderPassBeginInfo.pClearValues = &t_ClearColor;

	vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &t_RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Bind graphics pipeline
	vkCmdBindPipeline(m_CommandBuffers[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

	// Bind Vertex VertexBuffer
	const VkBuffer t_VertexBuffers[] = {m_VertexBuffer.GetBuffer()};
	const VkDeviceSize t_Offsets[] = {0};
	vkCmdBindVertexBuffers(m_CommandBuffers[m_CurrentFrame], 0, 1, t_VertexBuffers, t_Offsets);

	// Bind Index Buffer
	vkCmdBindIndexBuffer(m_CommandBuffers[m_CurrentFrame], m_IndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

	const VkExtent2D t_SwapChainExtent = m_SwapChain.GetExtent();

	// TODO store this somewhere for reuse
	// Set Viewport
	VkViewport t_Viewport = {};
	t_Viewport.x = 0.0f;
	t_Viewport.y = 0.0f;
	t_Viewport.width = static_cast<float>(t_SwapChainExtent.width);
	t_Viewport.height = static_cast<float>(t_SwapChainExtent.height);
	t_Viewport.minDepth = 0.0f;
	t_Viewport.maxDepth = 1.0f;

	vkCmdSetViewport(m_CommandBuffers[m_CurrentFrame], 0, 1, &t_Viewport);

	// TODO store this somewhere for reuse
	// Set Scissors
	VkRect2D t_Scissor = {};
	t_Scissor.offset = {0,0};
	t_Scissor.extent = t_SwapChainExtent;

	vkCmdSetScissor(m_CommandBuffers[m_CurrentFrame], 0, 1, &t_Scissor);

	// Bind Descriptor Sets
	vkCmdBindDescriptorSets(m_CommandBuffers[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1,
	                        &m_DescriptorSets[m_CurrentFrame], 0, nullptr);

	// Draw
	vkCmdDrawIndexed(m_CommandBuffers[m_CurrentFrame], static_cast<uint32_t>(s_quad_indices.size()), 1, 0, 0, 0);


	// end render pass
	vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

	// finish recording the command buffer
	if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not record Command VertexBuffer!");
	}
}

/// <summary>	Creates a uniform buffer for each in flight frame. </summary>
void VRenderer::CreateUniformBuffers()
{
	m_UniformBuffers.resize(m_MaxInFlightFrames);

	for (size_t i = 0; i < m_MaxInFlightFrames; i++)
	{
		m_UniformBuffers[i].CreateUniformBuffer(m_Device);
	}
}

void VRenderer::UpdateUniformBuffers(uint32_t a_CurrentImage)
{

	// TODO implement better
	// delta time
	static auto  t_Start = std::chrono::high_resolution_clock::now();

	const auto t_CurrTime = std::chrono::high_resolution_clock::now();
	float t_Delta = std::chrono::duration<float, std::chrono::seconds::period>(t_CurrTime - t_Start).count();

	// TODO make cgamera class that calculates view & projection and contains near & far
	// TODO move model mat into individual mesh class

	UniformBufferObject t_UBO = {};
	t_UBO.m_Model = glm::rotate(glm::mat4(1.0f), t_Delta * glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
	t_UBO.m_View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	VkExtent2D t_SwapExtent = m_SwapChain.GetExtent();
	float t_AspectRatio = static_cast<float>(t_SwapExtent.width) / static_cast<float>(t_SwapExtent.height);
	t_UBO.m_Projection = glm::perspective(glm::radians(45.0f), t_AspectRatio, 0.1f, 10.f);

	// TODO remove (crutch to avoid image being upside down due to glm coordinate system)
	t_UBO.m_Projection[1][1] *= -1;

	m_UniformBuffers[a_CurrentImage].FillBuffer(t_UBO);
}

VkDescriptorPool VRenderer::CreateDescriptorPool(const int a_DescriptorCount, const VkDevice& a_LogicalDevice)
{
	VkDescriptorPoolSize t_PoolSize;
	t_PoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	t_PoolSize.descriptorCount = static_cast<uint32_t>(a_DescriptorCount);

	VkDescriptorPoolCreateInfo t_DescriptorPoolCreateInfo = {};
	t_DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	t_DescriptorPoolCreateInfo.poolSizeCount = 1;
	t_DescriptorPoolCreateInfo.pPoolSizes = &t_PoolSize;
	t_DescriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(a_DescriptorCount);

	VkDescriptorPool t_DescriptorPool;

	if (vkCreateDescriptorPool(a_LogicalDevice, &t_DescriptorPoolCreateInfo, nullptr, &t_DescriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Could not create Descriptor Pool!");
	}

	return t_DescriptorPool;
}

void VRenderer::CreateDescriptorSets(int a_Count, VkDevice a_LogicalDevice,
                                     VkDescriptorSetLayout& a_DescriptorSetLayout,
                                     VkDescriptorPool& a_DescriptorPool, std::vector<VkDescriptorSet>& a_DescriptorSets)
{
	// allocate descriptor sets
	std::vector<VkDescriptorSetLayout> t_DescriptorSetLayouts(a_Count, a_DescriptorSetLayout);

	VkDescriptorSetAllocateInfo t_AllocateInfo = {};
	t_AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	t_AllocateInfo.descriptorPool = a_DescriptorPool;
	t_AllocateInfo.descriptorSetCount = static_cast<uint32_t>(a_Count);
	t_AllocateInfo.pSetLayouts = t_DescriptorSetLayouts.data();

	a_DescriptorSets.resize(a_Count);
	if (vkAllocateDescriptorSets(a_LogicalDevice, &t_AllocateInfo, a_DescriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Error! Could not allocate Descriptor Sets!");
	}

	// populate descriptor sets
	for (size_t i = 0; i < a_Count; i++)
	{
		VkDescriptorBufferInfo t_BufferInfo = {};
		t_BufferInfo.buffer = m_UniformBuffers[i].GetBuffer();
		t_BufferInfo.offset = 0;
		t_BufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet t_DescriptorWrite = {};
		t_DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		t_DescriptorWrite.dstSet = a_DescriptorSets[i];
		t_DescriptorWrite.dstBinding = 0;
		t_DescriptorWrite.dstArrayElement = 0;
		t_DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		t_DescriptorWrite.descriptorCount = 1;
		t_DescriptorWrite.pBufferInfo = &t_BufferInfo;
		t_DescriptorWrite.pImageInfo = nullptr;
		t_DescriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(a_LogicalDevice, 1, &t_DescriptorWrite, 0 ,nullptr);
	}
}

void VRenderer::CreateSyncObjects()
{
	//resize semaphore & fence vectors
	m_ImageAcquiredSemaphores.resize(m_MaxInFlightFrames);
	m_RenderFinishedSemaphores.resize(m_MaxInFlightFrames);
	m_InFlightFences.resize(m_MaxInFlightFrames);


	VkSemaphoreCreateInfo t_SemaphoreCreateInfo = {};
	t_SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo t_FenceCreateInfo = {};
	t_FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// create fence signaled to avoid waiting for it forever on the first frame
	t_FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; 

	// for each frame in flight
	for (int i = 0; i < m_MaxInFlightFrames; i++)
	{
		// create Semaphores
		if (vkCreateSemaphore(m_Device.GetLogicalDevice(), &t_SemaphoreCreateInfo, nullptr, &m_ImageAcquiredSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_Device.GetLogicalDevice(), &t_SemaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not create Semaphores!");
		}

		// create Fence
		if (vkCreateFence(m_Device.GetLogicalDevice(), &t_FenceCreateInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not create Fence!");
		}
	}
}

void VRenderer::DestroySyncObjects()
{
	for (int i = 0; i < m_MaxInFlightFrames; i++)
	{
		vkDestroySemaphore(m_Device.GetLogicalDevice(), m_ImageAcquiredSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device.GetLogicalDevice(), m_RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_Device.GetLogicalDevice(), m_InFlightFences[i], nullptr);
	}
}