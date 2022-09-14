#include "pch.h"
#include "vRenderer/vRenderer.h"
#include "vRenderer/helper_structs/RenderingHelpers.h"
#include "vRenderer/helpers/helpers.h"
#include "vRenderer/helpers/VulkanHelpers.h"

#define GLFW_INCLUDE_VULKAN
#include <algorithm>
#include <GLFW/glfw3.h>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

VRenderer::VRenderer(): m_Window(nullptr),
                        m_EnabledValidationLayers({"VK_LAYER_KHRONOS_validation"}),
						m_RequestedDeviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
{
}

bool VRenderer::Init(const int a_WindowWidth, const int a_WindowHeight)
{
	EnableValidation();

	InitGlfw(a_WindowWidth, a_WindowHeight);

	InitVulkan();

	return true;
}

bool VRenderer::Terminate()
{
	vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
	DestroyImageViews();
	vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
	vkDestroyDevice(m_LogicalDevice, nullptr);
	vkDestroySurfaceKHR(m_VInstance, m_WindowSurface, nullptr);
	vkDestroyInstance(m_VInstance, nullptr);
	glfwDestroyWindow(m_Window);
	return true;
}

void VRenderer::Render()
{
}

bool VRenderer::ShouldTerminate() const
{
	return glfwWindowShouldClose(m_Window);
}

void VRenderer::InitVulkan()
{
	CreateInstance();
	CreateWindowSurface();
	VkPhysicalDevice t_physicalDevice = ChoosePhysicalDevice();
	CreateLogicalDevice(t_physicalDevice);
	CreateSwapChain(t_physicalDevice);
	CreateImageViews();
	CreateGraphicsPipeline();
}

void VRenderer::InitGlfw(const int a_WindowWidth, const int a_WindowHeight)
{
	glfwInit();

	// tell GLFW not to create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// tell GFLW to not make the window resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(a_WindowWidth, a_WindowHeight, "vRenderer", nullptr, nullptr);
}

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

void VRenderer::EnableValidation() const
{
	// throw an exception if any of the requested validation layers are not available in the system
	CheckSupportedValidationLayers(m_EnabledValidationLayers);
}

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

//	todo make this function pick by choosing the GPU with the best rating 
VkPhysicalDevice VRenderer::ChoosePhysicalDevice()
{
	VkPhysicalDevice t_PhysicalDevice = VK_NULL_HANDLE;
	
	// query the amount of graphics cards available
	uint32_t t_NumDevices = 0;
	vkEnumeratePhysicalDevices(m_VInstance, &t_NumDevices, nullptr);

	// throw exception if there is no Graphics card supporting Vulkan
	if (t_NumDevices == 0) throw std::runtime_error("Could not find GPUs supporting Vulkan!");

	// store the handles to all found GPUs in an array
	std::vector<VkPhysicalDevice> t_DeviceHandles(t_NumDevices);
	vkEnumeratePhysicalDevices(m_VInstance, &t_NumDevices, t_DeviceHandles.data());

	// loop over the physical devices and choose the first on that fulfills the requirements
	for (const auto t_Device : t_DeviceHandles)
	{
		if (CheckDeviceSuitability(t_Device))
		{
			t_PhysicalDevice = t_Device;
			break;
		}
	}

	// throw a runtime error if none of the physical devices fulfill the requirements
	if (t_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("No suitable GPU found!");
	}

	return t_PhysicalDevice;
}

// todo make this a system that rates the GPUs instead
bool VRenderer::CheckDeviceSuitability(const VkPhysicalDevice a_Device)
{

	// check whether the operations required by this application are supported by the devices queue families
	const SupportedQueueFamilies t_SupportedQueueFamilies = CheckSupportedQueueFamilies(a_Device);

	return	t_SupportedQueueFamilies.IsComplete() && 
			CheckDeviceExtensionSupport(a_Device) && 
			CheckSwapChainCompatibility(a_Device);
}

bool VRenderer::CheckDeviceExtensionSupport(const VkPhysicalDevice a_Device) const
{
	// enumerate extension properties
	uint32_t t_NumExtensions = 0;
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_NumExtensions, nullptr);

	// get extension properties and store them
	std::vector<VkExtensionProperties> t_AvailableExtensions(t_NumExtensions);
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_NumExtensions, t_AvailableExtensions.data());

	std::set<std::string> t_ExtensionsRequested(m_RequestedDeviceExtensions.begin(), m_RequestedDeviceExtensions.end());

	for (const auto& t_Extension : t_AvailableExtensions)
	{
		t_ExtensionsRequested.erase(t_Extension.extensionName);
	}

	return t_ExtensionsRequested.empty();
}

SupportedQueueFamilies VRenderer::CheckSupportedQueueFamilies(const VkPhysicalDevice a_Device)
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

		if (CheckQueueFamilySupportsPresentation(a_Device, i))
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

bool VRenderer::CheckQueueFamilySupportedOperations(VkQueueFamilyProperties a_QueueFamilyProperty)
{
	// Check for supported operations.
	if (a_QueueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
	{
		return true;
	}

	return false;
}

bool VRenderer::CheckQueueFamilySupportsPresentation(const VkPhysicalDevice a_Device, const uint32_t a_QueueFamilyIndex) const
{
	// check whether the queue family supports presenting to the Window Surface
	VkBool32 t_SupportsPresenting = false;
	vkGetPhysicalDeviceSurfaceSupportKHR(a_Device, a_QueueFamilyIndex, m_WindowSurface, &t_SupportsPresenting);

	if (t_SupportsPresenting)
	{
		return true;
	}

	return false;
}

void VRenderer::CreateLogicalDevice(VkPhysicalDevice& a_PhysicalDevice)
{
	// set the number of queues used from each queue family
	SupportedQueueFamilies t_QueueFamilies = CheckSupportedQueueFamilies(a_PhysicalDevice);

	// create vector of creation info structs for all unique queue families
	std::vector<VkDeviceQueueCreateInfo> t_QueueCreateInfos;
	std::set<uint32_t> t_UniqueQueueFamilies = {t_QueueFamilies.m_GraphicsFamily.value(), t_QueueFamilies.m_PresentFamily.value()};

	float t_QueuePriorities = 1.0f;
	for (uint32_t t_QueueFamily : t_UniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo t_QueueInfo = {};
		t_QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		t_QueueInfo.queueFamilyIndex = t_QueueFamily;
		t_QueueInfo.queueCount = 1;
		t_QueueInfo.pQueuePriorities = &t_QueuePriorities;

		// add the newly created queue create info to the vector
		t_QueueCreateInfos.push_back(t_QueueInfo);
	}

	VkDeviceQueueCreateInfo t_DeviceQueueCreateInfo = {};
	t_DeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	t_DeviceQueueCreateInfo.queueFamilyIndex = t_QueueFamilies.m_GraphicsFamily.value();

	// only one queue (with graphics capabilities) is currently required as all command buffers
	// can be created on separate threats and then submitted all at once
	t_DeviceQueueCreateInfo.queueCount = 1;

	constexpr float t_QueuePriority = 1.0f;
	t_DeviceQueueCreateInfo.pQueuePriorities = &t_QueuePriority;

	// TODO specify the actual features later when they become relevant
	VkPhysicalDeviceFeatures t_PhysicalDeviceFeatures = {};

	// create the logical device
	VkDeviceCreateInfo t_LogicalDeviceCreateInfo = {};
	t_LogicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	t_LogicalDeviceCreateInfo.pQueueCreateInfos = t_QueueCreateInfos.data();
	t_LogicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(t_QueueCreateInfos.size());

	t_LogicalDeviceCreateInfo.pEnabledFeatures = &t_PhysicalDeviceFeatures;

	t_LogicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_RequestedDeviceExtensions.size());
	t_LogicalDeviceCreateInfo.ppEnabledExtensionNames = m_RequestedDeviceExtensions.data();

#ifdef _DEBUG
	t_LogicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_EnabledValidationLayers.size());
	t_LogicalDeviceCreateInfo.ppEnabledLayerNames = m_EnabledValidationLayers.data();
#else
	t_logicalDeviceCreateInfo.enabledLayerCount = 0;
#endif

	if(vkCreateDevice(a_PhysicalDevice, &t_LogicalDeviceCreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	// create Graphics Queue, store the queue handles for later use
	vkGetDeviceQueue(m_LogicalDevice, t_QueueFamilies.m_GraphicsFamily.value(), 0, &m_GraphicsQueue);

	// create present queue, store the queue handle for later use
	// vkGetDeviceQueue(m_LogicalDevice, t_queueFamilies.m_PresentFamily.value(), 0, &m_PresentQueue);
}

void VRenderer::CreateWindowSurface()
{
	if (glfwCreateWindowSurface(m_VInstance, m_Window, nullptr, &m_WindowSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("Window surface could not be created!");
	}
}

SwapChainInformation VRenderer::GetSwapChainInformation(const VkPhysicalDevice a_Device) const
{
	SwapChainInformation t_SwapChainInfo = {};

	// query surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(a_Device, m_WindowSurface, &t_SwapChainInfo.m_SurfaceCapabilities);

	// query supported surface formats
	uint32_t t_NumSupportedFormats = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, m_WindowSurface, &t_NumSupportedFormats, nullptr);

	if (t_NumSupportedFormats != 0)
	{
		t_SwapChainInfo.m_SupportedSurfaceFormats.resize(t_NumSupportedFormats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, m_WindowSurface, &t_NumSupportedFormats,
		                                     t_SwapChainInfo.m_SupportedSurfaceFormats.data());
	}

	// query supported presentation modes
	uint32_t t_NumSupportedPresentationModes = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, m_WindowSurface, &t_NumSupportedPresentationModes, nullptr);

	if (t_NumSupportedPresentationModes != 0)
	{
		t_SwapChainInfo.m_SupportedPresentModes.resize(t_NumSupportedPresentationModes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, m_WindowSurface, &t_NumSupportedPresentationModes,
		                                          t_SwapChainInfo.m_SupportedPresentModes.data());
	}

	return  t_SwapChainInfo;
}

bool VRenderer::CheckSwapChainCompatibility(const VkPhysicalDevice a_Device) const
{
	const SwapChainInformation t_SwapChainInfo = GetSwapChainInformation(a_Device);

	return	!t_SwapChainInfo.m_SupportedPresentModes.empty() &&
			!t_SwapChainInfo.m_SupportedSurfaceFormats.empty();
}

VkSurfaceFormatKHR VRenderer::PickSwapChainSurfaceFormat(
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

VkPresentModeKHR VRenderer::PickSwapChainPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes)
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

VkExtent2D VRenderer::PickSwapExtent(const VkSurfaceCapabilitiesKHR& a_SurfaceCapabilities) const
{
	if (a_SurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return a_SurfaceCapabilities.currentExtent;
	}

	// if the width of the current extent does exceed uint32_t max
	// create a new extent using the actual width and height
	int t_Width;
	int t_Height;

	glfwGetFramebufferSize(m_Window, &t_Width, &t_Height);

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

void VRenderer::CreateSwapChain(const VkPhysicalDevice a_Device)
{
	SwapChainInformation t_SwapChainInfo = GetSwapChainInformation(a_Device);

	// query whether optimal swap chain format, present mode and extent are available
	VkSurfaceFormatKHR t_SurfaceFormat = PickSwapChainSurfaceFormat(t_SwapChainInfo.m_SupportedSurfaceFormats);
	VkPresentModeKHR t_PresentMode = PickSwapChainPresentMode(t_SwapChainInfo.m_SupportedPresentModes);
	VkExtent2D t_Extent = PickSwapExtent(t_SwapChainInfo.m_SurfaceCapabilities);

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
	t_SwapChainCreateInfo.surface = m_WindowSurface;

	// define actual swap chain details
	t_SwapChainCreateInfo.minImageCount = t_MinImageCount;
	t_SwapChainCreateInfo.imageFormat = t_SurfaceFormat.format;
	t_SwapChainCreateInfo.imageColorSpace = t_SurfaceFormat.colorSpace;
	t_SwapChainCreateInfo.imageExtent = t_Extent;
	t_SwapChainCreateInfo.imageArrayLayers = 1;
	t_SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// define how the swap chain is supposed to handle images shared between multiple queues
	SupportedQueueFamilies t_SupportedQueueFamilies = CheckSupportedQueueFamilies(a_Device);
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

	if (vkCreateSwapchainKHR(m_LogicalDevice, &t_SwapChainCreateInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Swap Chain!");
	}

	// store the handles to the images contained in the Swap Chain
	uint32_t t_NumSwapChainImages = 0;
	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &t_NumSwapChainImages, nullptr);
	m_SwapChainImages.resize(t_NumSwapChainImages);
	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &t_NumSwapChainImages, m_SwapChainImages.data());

	// store swap chain format and extent
	m_SwapChainFormat = t_SurfaceFormat.format;
	m_SwapChainExtent = t_Extent;
}

void VRenderer::CreateImageViews()
{
	// resize the SwapChainImageViews vector to contain enough space for the amount 
	// of images in the Swap Chain
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	// loop over each Swap Chain Image and create an Image View for it
	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
	{
		VkImageViewCreateInfo t_ImageViewCreateInfo = {};

		t_ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		t_ImageViewCreateInfo.image = m_SwapChainImages[i];

		// specify how image data should be interpreted
		t_ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		t_ImageViewCreateInfo.format = m_SwapChainFormat;

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

		if (vkCreateImageView(m_LogicalDevice, &t_ImageViewCreateInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not create image view!");
		}
	}
}

void VRenderer::DestroyImageViews()
{
	if (m_SwapChainImageViews.size() == 0)
	{
#ifdef _DEBUG
		std::cout << "No image views to destroy. Returning early." << std::endl;
#endif
		return;
	}

	for (size_t i = 0; 0 < m_SwapChainImageViews.size(); i++)
	{
		vkDestroyImageView(m_LogicalDevice, m_SwapChainImageViews[0], nullptr);

#ifdef _DEBUG
		std::cout << "Destroying Image View: " << i << "of" << m_SwapChainImageViews.size() <<std::endl;
#endif
	}

#ifdef _DEBUG
		std::cout << "Image views destroyed successfully." << std::endl;
#endif
}

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
	t_FragShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
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
	VkPipelineVertexInputStateCreateInfo t_VertexInputStateCreateInfo = GenVertexInputStateCreateInfo();

	// generate Input Assembly stage
	VkPipelineInputAssemblyStateCreateInfo t_InputAssemblyStateCreateInfo = GenInputAssemblyStateCreateInfo();

	// generate Viewport
	VkViewport t_Viewport = GenViewportData(m_SwapChainExtent);

	// generate scissor rectangle
	VkRect2D t_ScissorRect = {};
	t_ScissorRect.offset = {0,0};
	t_ScissorRect.extent = m_SwapChainExtent;

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
	VkPipelineLayoutCreateInfo t_PipelineLayoutCreateInfo = GenPipelineCreateInfo();

	if (vkCreatePipelineLayout(m_LogicalDevice, &t_PipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Pipeline Layout!");
	}



	// make Graphics Pipeline
	


	//destroy Shader modules at the end of the function
	vkDestroyShaderModule(m_LogicalDevice, t_VertexShader, nullptr);
	vkDestroyShaderModule(m_LogicalDevice, t_FragmentShader, nullptr);
}

VkShaderModule VRenderer::GenShaderModule(const std::vector<char>& a_CodeData) const
{
	VkShaderModuleCreateInfo t_ShaderModuleCreateInfo = {};
	t_ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	t_ShaderModuleCreateInfo.codeSize = a_CodeData.size();
	t_ShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(a_CodeData.data());

	VkShaderModule t_Module;
	if (vkCreateShaderModule(m_LogicalDevice, &t_ShaderModuleCreateInfo, nullptr, &t_Module) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create Shader Module!");
	}

	return t_Module;
}


