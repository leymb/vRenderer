#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <GLFW/glfw3.h>

#include "helper_structs/RenderingHelpers.h"


class Device;
class SwapChain
{
public:
	SwapChain();

	/// <summary>	Creates a SwapChain. </summary>
	/// <param name="a_Device">		  	The device.</param>
	/// <param name="a_WindowSurface">	The window surface.</param>
	/// <param name="a_Window">		  	[in,out] If non-null, the window.</param>

	void Create(const Device& a_Device, const VkSurfaceKHR& a_WindowSurface, GLFWwindow* a_Window);
	void Cleanup(VkDevice a_LogicalDevice, std::vector<VkFramebuffer>& a_FramebufferVector);

	VkSwapchainKHR GetSwapChain();

	void CreateImageViews(const VkDevice& a_LogicalDevice);
	void DestroyImageViews(VkDevice a_LogicalDevice);
	std::vector<VkImageView>& GetImageViews();

	/// <summary>
	/// 	Retrieves information about the Surface Formats, Present Modes and Surface Capabilities
	/// 	the Swap Chain supports.
	/// </summary>
	/// <param name="a_Device">	The physical device.</param>
	/// <param name="a_WindowSurface">The window surface.</param>
	/// <returns>
	/// 	A SwapChainInformation struct containing information about supported features.
	/// </returns>
	static SwapChainInformation GetSwapChainInformation(VkPhysicalDevice a_Device, const VkSurfaceKHR& a_WindowSurface);

	VkExtent2D GetExtent() const;

	VkFormat GetFormat();

private:

	VkSurfaceFormatKHR PickSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats) const;

	VkPresentModeKHR PickSwapChainPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes);

	VkExtent2D PickSwapExtent(const VkSurfaceCapabilitiesKHR& a_SurfaceCapabilities, GLFWwindow* a_Window) const;

	VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_Images;
	std::vector<VkImageView> m_ImageViews;
	VkFormat m_Format;
	VkExtent2D m_Extent;
};

