#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "VertexBuffer.h"
#include "Device.h"
#include "helper_structs/RenderingHelpers.h"

struct GLFWwindow;
struct SupportedQueueFamilies;

class VRenderer
{
public:
	VRenderer();

	/// <summary>	Initializes the Renderer. </summary>
	/// <param name="a_WindowWidth"> 	(Optional) Width of the window.</param>
	/// <param name="a_WindowHeight">	(Optional) Height of the window.</param>
	/// <returns>	True if it succeeds, false if it fails. </returns>

	bool Init(int a_WindowWidth = 800, int a_WindowHeight = 600);
	bool Terminate();
	void Render();

	bool ShouldTerminate() const;

	const int m_MaxInFlightFrames = 2;

private:
	void InitVulkan();

	/// <summary>	Initializes GLFW and creates a GLFWwindow. </summary>
	/// <param name="a_WindowWidth"> 	Width of the window.</param>
	/// <param name="a_WindowHeight">	Height of the window.</param>

	void InitGlfw(int a_WindowWidth, int a_WindowHeight);

	/// <summary>	Creates a Vulkan Instance and stores its handle in m_VInstance. </summary>
	bool CreateInstance();

	/// <summary>	Checks whether the provided list of extensions is supported. </summary>
	/// <returns>	True if it succeeds, false if it fails. </returns>

	static bool CheckExtensionSupport(uint32_t a_ExtensionCount, const char** a_Extensions);

	void GenInstanceCreateData(bool a_ValidationEnabled, InstanceCreateData& a_InstanceCreateData) const;

	/// <summary>	Enables validation layers. </summary>

	void EnableValidation() const;

	/// <summary>	Checks whether the requested valiadtion layers are supported. </summary>
	/// <param name="a_RequestedValidationLayers">	The requested validation layers.</param>
	/// <returns>
	/// 	True if the requested validation layers are supported, else throws a runtime exception.
	/// </returns>

	static bool CheckSupportedValidationLayers(const std::vector<const char*>& a_RequestedValidationLayers);

	void CreateWindowSurface();

	/// <summary>
	/// 	Retrieves information about the Surface Formats, Present Modes and Surface Capabilities
	/// 	the Swap Chain supports.
	/// </summary>
	/// <param name="a_Device">	The physical device.</param>
	/// <returns>
	/// 	A SwapChainInformation struct containing information about supported features.
	/// </returns>
	SwapChainInformation GetSwapChainInformation(const VkPhysicalDevice a_Device) const;

	bool CheckSwapChainCompatibility(const VkPhysicalDevice a_Device) const;

	/// <summary>	Picks the swap chain surface format described by a_AvailableFormats. </summary>
	/// <param name="a_AvailableFormats">	The available formats.</param>
	/// <returns>
	/// 	If found, returns a SwapChainFormat with a 32-bit SRGB format the SRGB color space
	/// 	(VK_FORMAT_B8G8R8A8_SRGB and VK_COLOR_SPACE_SRGB_NONLINEAR_KHR). If no format is found
	/// 	that satisfies these conditions, returns the first element found in a_AvailableFormats.
	/// </returns>

	VkSurfaceFormatKHR PickSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats) const;

	/// <summary>
	/// 	Picks the most suitable swap chain present mode found in a_AvailablePresentModes.
	/// </summary>
	/// <param name="a_AvailablePresentModes">	The available present modes.</param>
	/// <returns>
	/// 	If found in the available present modes, returns VK_PRESENT_MODE_MAILBOX_KHR. If not,
	/// 	returns VK_PRESENT_MODE_FIFO_KHR.
	/// </returns>

	VkPresentModeKHR PickSwapChainPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes);

	/// <summary>	Picks the extent of the swap chain. </summary>
	/// <param name="a_SurfaceCapabilities">	The surface capabilities.</param>
	/// <returns>	Returns a VkExtent2D. </returns>

	VkExtent2D PickSwapExtent(const VkSurfaceCapabilitiesKHR& a_SurfaceCapabilities) const;

	void CreateSwapChain(VkPhysicalDevice a_Device);

	void CreateImageViews();

	void DestroyImageViews();

	/// <summary>
	/// 	Creates the graphics pipeline and the required fixed stage functions, render passes,
	/// 	viewport and scissor descriptions.
	/// </summary>

	void CreateGraphicsPipeline();

	/// <summary>	Generates a shader module from the provided vector of bytecode. </summary>
	/// <param name="a_CodeData">	The shader bytecode loaded from a compiled .spv file.</param>
	/// <returns>	The shader module. </returns>

	VkShaderModule GenShaderModule(const std::vector<char>& a_CodeData);

	/// <summary>	Creates a render pass. </summary>
	void CreateRenderPass();

	void CreateFrameBuffers();

	void CreateCommandPool();

	void CreateCommandBuffers();

	/// <summary>	Records commands to a command buffer. </summary>
	/// <param name="a_CommandBuffer">	Command VertexBuffer.</param>
	/// <param name="a_ImageIndex">   	Zero-based index of the image.</param>

	void RecordCommandBuffer(VkCommandBuffer a_CommandBuffer, uint32_t a_ImageIndex);

	void CreateSyncObjects();

	void DestroySyncObjects();

	// GLFW members
	GLFWwindow* m_Window;
	VkSurfaceKHR m_WindowSurface;

	// Vulkan members
	VkInstance m_VInstance = nullptr;

	// TODO change these to be passed to the constructor instead to allow for outside definitions of the extensions to use
	const std::vector<const char*> m_EnabledValidationLayers{};
	const std::vector<const char*> m_RequestedDeviceExtensions{};

	Device m_Device;

	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;

	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages;
	std::vector<VkImageView> m_SwapChainImageViews;
	VkFormat m_SwapChainFormat;
	VkExtent2D m_SwapChainExtent;

	VkRenderPass m_MainRenderPass;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;

	std::vector<VkFramebuffer> m_Framebuffers;

	VkCommandPool m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	std::vector<VkSemaphore> m_ImageAcquiredSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	uint32_t m_CurrentFrame = 0;

	VertexBuffer m_VertexBuffer;
};

