#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <vulkan/vulkan_core.h>

#include "Buffer/VertexBuffer.h"
#include "Device.h"
#include "helper_structs/RenderingHelpers.h"
#include <vRenderer/SwapChain.h>

#include "Model.h"
#include "Texture.h"
#include "Buffer/IndexBuffer.h"
#include "Buffer/UniformBuffer.h"

class Camera;
struct GLFWwindow;
struct SupportedQueueFamilies;

class VRenderer
{
public:
	VRenderer();

	/// <summary>	Initializes the Renderer. </summary>
	/// <param name="a_WindowWidth">			  	(Optional) Width of the window.</param>
	/// <param name="a_WindowHeight">			  	(Optional) Height of the window.</param>
	/// <param name="a_EnabledValidationLayers">  	(Optional) The enabled validation layers.</param>
	/// <param name="a_RequestedDeviceExtensions">	(Optional) The requested device extensions.</param>
	/// <returns>	True if it succeeds, false if it fails. </returns>

	bool Init(int a_WindowWidth = 800, int a_WindowHeight = 600,
	          const std::vector<const char*>& a_EnabledValidationLayers = {"VK_LAYER_KHRONOS_validation"},
	          const std::vector<const char*>& a_RequestedDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME});

	bool Terminate();
	void Render(Camera& a_Camera);

	bool ShouldTerminate() const;

	/// <summary>	Gets window extent. </summary>
	/// <returns>	The window extent in the format of width, height. </returns>

	glm::ivec2 GetWindowExtent();

	const int m_MaxInFlightFrames = 2;

private:
	void InitVulkan();

	void InitGlfw(int a_WindowWidth, int a_WindowHeight);

	bool CreateInstance();


	static bool CheckExtensionSupport(uint32_t a_ExtensionCount, const char** a_Extensions);

	void GenInstanceCreateData(bool a_ValidationEnabled, InstanceCreateData& a_InstanceCreateData) const;


	void EnableValidation() const;

	static bool CheckSupportedValidationLayers(const std::vector<const char*>& a_RequestedValidationLayers);


	void CreateWindowSurface();

	void CreateGraphicsPipeline();

	VkShaderModule GenShaderModule(const std::vector<char>& a_CodeData);


	void CreateRenderPass();

	void CreateFrameBuffers();

	void CreateCommandPool();

	void CreateCommandBuffers();

	void RecordCommandBuffer(VkCommandBuffer a_CommandBuffer, uint32_t a_ImageIndex);

	void CreateUniformBuffers();
	void UpdateUniformBuffers(uint32_t a_CurrentImage, Camera& a_Camera);

	VkDescriptorPool CreateDescriptorPool(int a_DescriptorCount, const VkDevice& a_LogicalDevice);
	void CreateDescriptorSets(int a_Count, VkDevice a_LogicalDevice, VkDescriptorSetLayout& a_DescriptorSetLayout,
	                          VkDescriptorPool& a_DescriptorPool, std::vector<VkDescriptorSet>& a_DescriptorSets);


	void CreateSyncObjects();

	void DestroySyncObjects();

	// Depth Buffering
	void CreateDepthResources();

	// MSAA
	void CreateColorResources();

	void HandleResize();

	// GLFW members
	GLFWwindow* m_Window;
	VkSurfaceKHR m_WindowSurface;

	// Vulkan members
	VkInstance m_VInstance = nullptr;

	std::vector<const char*> m_EnabledValidationLayers{};
	std::vector<const char*> m_RequestedDeviceExtensions{};

	Device m_Device;
	SwapChain m_SwapChain;

	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;

	VkRenderPass m_MainRenderPass;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;

	std::vector<UniformBuffer> m_UniformBuffers{};
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	std::vector<VkFramebuffer> m_Framebuffers;

	VkCommandPool m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	std::vector<VkSemaphore> m_ImageAcquiredSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	uint32_t m_CurrentFrame = 0;

	VertexBuffer m_VertexBuffer;
	IndexBuffer m_IndexBuffer;

	Model m_TestModel;

	Image m_DepthImage;

	// used for MSAA
	Image m_ColorImage;

	bool m_FrameBufferResized = false;
};