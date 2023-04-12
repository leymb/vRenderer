#include "pch.h"
#include "vRenderer/SwapChain.h"

#include <algorithm>
#include <iostream>

#include <vRenderer/Device.h>
#include "vRenderer/helpers/VulkanHelpers.h"
#include "vRenderer/helper_structs/RenderingHelpers.h"
#include <stdexcept>
#include <GLFW/glfw3.h>

SwapChain::SwapChain()
{
}

void SwapChain::Create(const Device& a_Device, const VkSurfaceKHR& a_WindowSurface, GLFWwindow* a_Window)
{
	SwapChainInformation t_SwapChainInfo = GetSwapChainInformation(a_Device.GetPhysicalDevice(), a_WindowSurface);

	// query whether optimal swap chain format, present mode and extent are available
	VkSurfaceFormatKHR t_SurfaceFormat = PickSwapChainSurfaceFormat(t_SwapChainInfo.m_SupportedSurfaceFormats);
	VkPresentModeKHR t_PresentMode = PickSwapChainPresentMode(t_SwapChainInfo.m_SupportedPresentModes);
	VkExtent2D t_Extent = PickSwapExtent(t_SwapChainInfo.m_SurfaceCapabilities, a_Window);

	// request one more image than minimally required to avoid having to wait 
	// for the driver to complete internal operations before rendering 
	uint32_t t_MinImageCount = t_SwapChainInfo.m_SurfaceCapabilities.minImageCount + 1;

	if (t_SwapChainInfo.m_SurfaceCapabilities.maxImageCount > 0 &&
		t_MinImageCount > t_SwapChainInfo.m_SurfaceCapabilities.maxImageCount)
	{
		t_MinImageCount = t_SwapChainInfo.m_SurfaceCapabilities.maxImageCount;
	}

	// generate Swap Chain create info & specify which surface the swap chain
	// is associated with
	VkSwapchainCreateInfoKHR t_SwapChainCreateInfo = {};
	t_SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	t_SwapChainCreateInfo.surface = a_WindowSurface;

	// define actual swap chain details
	t_SwapChainCreateInfo.minImageCount = t_MinImageCount;
	t_SwapChainCreateInfo.imageFormat = t_SurfaceFormat.format;
	t_SwapChainCreateInfo.imageColorSpace = t_SurfaceFormat.colorSpace;
	t_SwapChainCreateInfo.imageExtent = t_Extent;
	t_SwapChainCreateInfo.imageArrayLayers = 1;
	t_SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// define how the swap chain is supposed to handle images shared between multiple queues
	SupportedQueueFamilies t_SupportedQueueFamilies = CheckSupportedQueueFamilies(a_Device.GetPhysicalDevice(), a_WindowSurface);
	uint32_t t_QueueFamilyIndices[] = {
		t_SupportedQueueFamilies.m_GraphicsFamily.value(), t_SupportedQueueFamilies.m_PresentFamily.value()
	};

	// if the families of the Graphics Queue and the Present Queue are different
	if (t_SupportedQueueFamilies.m_GraphicsFamily != t_SupportedQueueFamilies.m_PresentFamily)
	{
		// set image and its sub-resources to be accessible from multiple queues
		// and add these queues to the create info
		t_SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		t_SwapChainCreateInfo.queueFamilyIndexCount = 2;
		t_SwapChainCreateInfo.pQueueFamilyIndices = t_QueueFamilyIndices;
	}
	else
	{
		// limit the accessibility of the image and its sub-resources to one queue exclusively
		// requires to explicitly transfer ownership if another queue family is to use it
		t_SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		t_SwapChainCreateInfo.queueFamilyIndexCount = 0;
		t_SwapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	t_SwapChainCreateInfo.preTransform = t_SwapChainInfo.m_SurfaceCapabilities.currentTransform;

	// set Swap Chain to ignore the alpha channel
	t_SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	t_SwapChainCreateInfo.presentMode = t_PresentMode;

	// set Swap Chain to ignore obscured pixels
	t_SwapChainCreateInfo.clipped = VK_TRUE;

	t_SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(a_Device.GetLogicalDevice(), &t_SwapChainCreateInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Swap Chain!");
	}

	// store the handles to the images contained in the Swap Chain
	uint32_t t_NumSwapChainImages = 0;
	vkGetSwapchainImagesKHR(a_Device.GetLogicalDevice(), m_SwapChain, &t_NumSwapChainImages, nullptr);
	m_Images.resize(t_NumSwapChainImages);
	vkGetSwapchainImagesKHR(a_Device.GetLogicalDevice(), m_SwapChain, &t_NumSwapChainImages, m_Images.data());

	// store swap chain format and extent
	m_Format = t_SurfaceFormat.format;
	m_Extent = t_Extent;
}

void SwapChain::Cleanup(VkDevice a_LogicalDevice, std::vector<VkFramebuffer>& a_FramebufferVector)
{
	DestroyFrameBuffers(a_FramebufferVector, a_LogicalDevice);
	DestroyImageViews(a_LogicalDevice);
	vkDestroySwapchainKHR(a_LogicalDevice, m_SwapChain, nullptr);
}

VkSwapchainKHR SwapChain::GetSwapChain()
{
	return m_SwapChain;
}

void SwapChain::CreateImageViews(const VkDevice& a_LogicalDevice)
{
	// resize the SwapChainImageViews vector to contain enough space for the amount 
	// of images in the Swap Chain
	m_ImageViews.resize(m_Images.size());

	// loop over each Swap Chain Image and create an Image View for it
	for (size_t i = 0; i < m_ImageViews.size(); i++)
	{
		VkImageViewCreateInfo t_ImageViewCreateInfo = {};

		t_ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		t_ImageViewCreateInfo.image = m_Images[i];

		// specify how image data should be interpreted
		t_ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		t_ImageViewCreateInfo.format = m_Format;

		// set swizzling to default mapping
		t_ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		t_ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		t_ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		t_ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		t_ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		t_ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		t_ImageViewCreateInfo.subresourceRange.levelCount = 1;
		t_ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		t_ImageViewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(a_LogicalDevice, &t_ImageViewCreateInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not create image view!");
		}
	}
}

void SwapChain::DestroyImageViews(VkDevice a_LogicalDevice)
{
	if (m_ImageViews.size() == 0)
	{
#ifdef _DEBUG
		std::cout << "No image views to destroy. Returning early." << std::endl;
#endif
		return;
	}

#ifdef _DEBUG
	int t_Counter = 1;
#endif

	for (VkImageView t_ImageView : m_ImageViews)
	{
		vkDestroyImageView(a_LogicalDevice, t_ImageView, nullptr);

#ifdef _DEBUG
		std::cout << "Destroying Image View: " << t_Counter << " of " << m_ImageViews.size() <<std::endl;
		t_Counter++;
#endif
	}

#ifdef _DEBUG
		std::cout << "Image views destroyed successfully." << std::endl;
#endif
}

std::vector<VkImageView>& SwapChain::GetImageViews()
{
	return m_ImageViews;
}

SwapChainInformation SwapChain::GetSwapChainInformation(const VkPhysicalDevice a_Device, const VkSurfaceKHR& a_WindowSurface)
{
	SwapChainInformation t_SwapChainInfo = {};

	// query surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(a_Device, a_WindowSurface, &t_SwapChainInfo.m_SurfaceCapabilities);

	// query supported surface formats
	uint32_t t_NumSupportedFormats = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, a_WindowSurface, &t_NumSupportedFormats, nullptr);

	if (t_NumSupportedFormats != 0)
	{
		t_SwapChainInfo.m_SupportedSurfaceFormats.resize(t_NumSupportedFormats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, a_WindowSurface, &t_NumSupportedFormats,
		                                     t_SwapChainInfo.m_SupportedSurfaceFormats.data());
	}

	// query supported presentation modes
	uint32_t t_NumSupportedPresentationModes = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, a_WindowSurface, &t_NumSupportedPresentationModes, nullptr);

	if (t_NumSupportedPresentationModes != 0)
	{
		t_SwapChainInfo.m_SupportedPresentModes.resize(t_NumSupportedPresentationModes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, a_WindowSurface, &t_NumSupportedPresentationModes,
		                                          t_SwapChainInfo.m_SupportedPresentModes.data());
	}

	return  t_SwapChainInfo;
}

VkExtent2D SwapChain::GetExtent() const
{
	return m_Extent;
}

VkFormat SwapChain::GetFormat()
{
	return m_Format;
}

/// <summary>	Picks the swap chain surface format described by a_AvailableFormats. </summary>
/// <param name="a_AvailableFormats">	The available formats.</param>
/// <returns>
/// 	If found, returns a SwapChainFormat with a 32-bit SRGB format the SRGB color space
/// 	(VK_FORMAT_B8G8R8A8_SRGB and VK_COLOR_SPACE_SRGB_NONLINEAR_KHR). If no format is found
/// 	that satisfies these conditions, returns the first element found in a_AvailableFormats.
/// </returns>

VkSurfaceFormatKHR SwapChain::PickSwapChainSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats) const
{
	// loop over all formats and check for a 32-bit  SRGB format and and SRGB color space
	for (const VkSurfaceFormatKHR t_Format : a_AvailableFormats)
	{
		if (t_Format.format == VK_FORMAT_B8G8R8A8_SRGB && 
			t_Format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return t_Format;
		}
	}

	// if no suitable format is found, simply return the first available format
	return a_AvailableFormats[0];
}

/// <summary>
/// 	Picks the most suitable swap chain present mode found in a_AvailablePresentModes.
/// </summary>
/// <param name="a_AvailablePresentModes">	The available present modes.</param>
/// <returns>
/// 	If found in the available present modes, returns VK_PRESENT_MODE_MAILBOX_KHR. If not,
/// 	returns VK_PRESENT_MODE_FIFO_KHR.
/// </returns>

VkPresentModeKHR SwapChain::PickSwapChainPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes)
{
	for (const VkPresentModeKHR& t_PresentMode: a_AvailablePresentModes)
	{
		if (t_PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return t_PresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

/// <summary>	Picks the extent of the swap chain. </summary>
/// <param name="a_SurfaceCapabilities">	The surface capabilities.</param>
/// <param name="a_Window">					[in,out] If non-null, the window.</param>
/// <returns>	Returns a VkExtent2D. </returns>

VkExtent2D SwapChain::PickSwapExtent(const VkSurfaceCapabilitiesKHR& a_SurfaceCapabilities, GLFWwindow* a_Window) const
{
	if (a_SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return a_SurfaceCapabilities.currentExtent;
	}

	// if the width of the current extent does exceed uint32_t max
	// create a new extent using the actual width and height
	int t_Width;
	int t_Height;

	glfwGetFramebufferSize(a_Window, &t_Width, &t_Height);

	VkExtent2D t_ActualExtent = 
	{
		static_cast<uint32_t>(t_Width),
		static_cast<uint32_t>(t_Height)
	};

	t_ActualExtent.width = std::clamp(t_ActualExtent.width, a_SurfaceCapabilities.minImageExtent.width,
		                                  a_SurfaceCapabilities.maxImageExtent.width);

	t_ActualExtent.height = std::clamp(t_ActualExtent.height, a_SurfaceCapabilities.minImageExtent.height,
		                                  a_SurfaceCapabilities.maxImageExtent.height);

	return t_ActualExtent;
}
