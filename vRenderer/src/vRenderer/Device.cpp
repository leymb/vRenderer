#include "pch.h"
#include "vRenderer/Device.h"
#include "../include/vRenderer/helper_structs/RenderingHelpers.h"

#include <iostream>
#include <set>

#include "vRenderer/helpers/VulkanHelpers.h"
#include <vRenderer/SwapChain.h>

Device::Device()
{
	
}

VkDevice Device::GetLogicalDevice() const
{
	return m_LogicalDevice;
}

VkSampleCountFlagBits Device::GetMSAASampleCount() const
{
	return m_MSAASampleCount;
}

/// <summary>
/// 	This function queries all existing physical devices (graphics cards) and chooses the
/// 	first one that suits the provided requirements.
/// </summary>
/// <returns>	The chosen physical device. </returns>

//	todo make this function pick by choosing the GPU with the best rating 
VkPhysicalDevice Device::ChoosePhysicalDevice(VkInstance& a_Instance, VkSurfaceKHR a_Surface,
                                              const std::vector<const char*>& a_RequestedDeviceExtensions)
{
	VkPhysicalDevice t_PhysicalDevice = VK_NULL_HANDLE;
	
	// query the amount of graphics cards available
	uint32_t t_NumDevices = 0;
	vkEnumeratePhysicalDevices(a_Instance, &t_NumDevices, nullptr);

	// throw exception if there is no Graphics card supporting Vulkan
	if (t_NumDevices == 0) throw std::runtime_error("Could not find GPUs supporting Vulkan!");

	// store the handles to all found GPUs in an array
	std::vector<VkPhysicalDevice> t_DeviceHandles(t_NumDevices);
	vkEnumeratePhysicalDevices(a_Instance, &t_NumDevices, t_DeviceHandles.data());

	// loop over the physical devices and choose the first on that fulfills the requirements
	for (const auto t_Device : t_DeviceHandles)
	{
		if (CheckDeviceSuitability(t_Device, a_Surface, a_RequestedDeviceExtensions))
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

	VkPhysicalDeviceProperties t_DeviceProperties;
	vkGetPhysicalDeviceProperties(t_PhysicalDevice,&t_DeviceProperties);

	m_PhysicalDevice = t_PhysicalDevice;
	m_MSAASampleCount = CheckMSAASampleCount(t_PhysicalDevice);

#ifdef _DEBUG
	std::cout << "Chose " << t_DeviceProperties.deviceName << " as physical device." << std::endl;
	std::cout << "Physical Device supports up to " << m_MSAASampleCount << " MSAA Samples" << std::endl;
#endif

	return t_PhysicalDevice;
}

/// <summary>	Checks device suitability. </summary>
/// <param name="a_Device">					  	The device.</param>
/// <param name="a_Surface">				  	The Window Surface.</param>
/// <param name="a_RequestedDeviceExtensions">	The Device Extensions Required.</param>
/// <param name="a_SwapChain">				  	[in,out] The Renderer's SwapChain.</param>
/// <returns>
/// 	Returns true if the device supports queue families that in turn support the operations
/// 	required by this application, returns false if not.
/// </returns>

bool Device::CheckDeviceSuitability(const VkPhysicalDevice a_Device, const VkSurfaceKHR a_Surface,
                                    const std::vector<const char*>& a_RequestedDeviceExtensions) const
{

	// check whether the operations required by this application are supported by the devices queue families
	const SupportedQueueFamilies t_SupportedQueueFamilies = CheckSupportedQueueFamilies(a_Device, a_Surface);

	// check device features
	VkPhysicalDeviceFeatures t_PhysicalDeviceFeatures = {};
	vkGetPhysicalDeviceFeatures(a_Device, &t_PhysicalDeviceFeatures);

	return	t_SupportedQueueFamilies.IsComplete() && 
			CheckDeviceExtensionSupport(a_Device, a_RequestedDeviceExtensions) && 
			CheckSwapChainCompatibility(a_Device, a_Surface) &&
			t_PhysicalDeviceFeatures.samplerAnisotropy;
}

bool Device::CheckSwapChainCompatibility(const VkPhysicalDevice& a_Device, const VkSurfaceKHR& a_WindowSurface)
{
	const auto [t_SurfaceCapabilities, t_SupportedSurfaceFormats, t_SupportedPresentModes] = SwapChain::GetSwapChainInformation(
		a_Device, a_WindowSurface);

	return	!t_SupportedPresentModes.empty() &&
			!t_SupportedSurfaceFormats.empty();
}

/// <summary>
/// 	Checks whether the physical device supports the extensions indicated in
/// 	m_RequestedDeviceExtensions.
/// </summary>
/// <param name="a_Device">	The physical device.</param>
/// <returns>	True if all extensions are supported, false if not. </returns>
bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice a_Device, const std::vector<const char*>& a_RequestedDeviceExtensions) const
{
	// enumerate extension properties
	uint32_t t_NumExtensions = 0;
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_NumExtensions, nullptr);

	// get extension properties and store them
	std::vector<VkExtensionProperties> t_AvailableExtensions(t_NumExtensions);
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_NumExtensions, t_AvailableExtensions.data());

	std::set<std::string> t_ExtensionsRequested(a_RequestedDeviceExtensions.begin(), a_RequestedDeviceExtensions.end());

	for (const auto& t_Extension : t_AvailableExtensions)
	{
		t_ExtensionsRequested.erase(t_Extension.extensionName);
	}

	return t_ExtensionsRequested.empty();
}

/// <summary>	Queries the maximum MSAA samples the physical device supports. </summary>
/// <returns>	The VkSampleCountFlagBits. </returns>

VkSampleCountFlagBits Device::CheckMSAASampleCount(VkPhysicalDevice& a_PhysicalDevice)
{
	VkPhysicalDeviceProperties t_PhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(a_PhysicalDevice, &t_PhysicalDeviceProperties);

	VkSampleCountFlags t_SampleCount = t_PhysicalDeviceProperties.limits.framebufferColorSampleCounts &
		t_PhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

	if (t_SampleCount & VK_SAMPLE_COUNT_64_BIT) {return VK_SAMPLE_COUNT_64_BIT;}
	if (t_SampleCount & VK_SAMPLE_COUNT_32_BIT) {return VK_SAMPLE_COUNT_32_BIT;}
	if (t_SampleCount & VK_SAMPLE_COUNT_16_BIT) {return VK_SAMPLE_COUNT_16_BIT;}
	if (t_SampleCount & VK_SAMPLE_COUNT_8_BIT) {return VK_SAMPLE_COUNT_8_BIT;}
	if (t_SampleCount & VK_SAMPLE_COUNT_4_BIT) {return VK_SAMPLE_COUNT_4_BIT;}
	if (t_SampleCount & VK_SAMPLE_COUNT_2_BIT) {return VK_SAMPLE_COUNT_2_BIT;}

	return VK_SAMPLE_COUNT_1_BIT;
}

//bool Device::CheckSwapChainCompatibility(VkPhysicalDevice a_Device, VkSurfaceKHR a_Surface) const
//{
//	const SwapChainInformation t_SwapChainInfo = GetSwapChainInformation(a_Device, a_Surface);
//
//	return	!t_SwapChainInfo.m_SupportedPresentModes.empty() &&
//			!t_SwapChainInfo.m_SupportedSurfaceFormats.empty();
//}

void Device::CreateLogicalDevice(VkSurfaceKHR a_Surface, VkQueue& a_GraphicsQueue, VkQueue& a_PresentQueue, const std::vector<const char*>& a_RequestedDeviceExtensions,
                                 const std::vector<const char*>& a_EnabledValidationLayers)
{
	if (m_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Error: You must choose a physical device before creating a logical device!\n");
	}


	// set the number of queues used from each queue family
	SupportedQueueFamilies t_QueueFamilies = CheckSupportedQueueFamilies(m_PhysicalDevice, a_Surface);

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
	t_PhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	t_PhysicalDeviceFeatures.sampleRateShading = VK_TRUE;

	// create the logical device
	VkDeviceCreateInfo t_LogicalDeviceCreateInfo = {};
	t_LogicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	t_LogicalDeviceCreateInfo.pQueueCreateInfos = t_QueueCreateInfos.data();
	t_LogicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(t_QueueCreateInfos.size());

	t_LogicalDeviceCreateInfo.pEnabledFeatures = &t_PhysicalDeviceFeatures;

	t_LogicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(a_RequestedDeviceExtensions.size());
	t_LogicalDeviceCreateInfo.ppEnabledExtensionNames = a_RequestedDeviceExtensions.data();

#ifdef _DEBUG
	t_LogicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(a_EnabledValidationLayers.size());
	t_LogicalDeviceCreateInfo.ppEnabledLayerNames = a_EnabledValidationLayers.data();
#else
	t_LogicalDeviceCreateInfo.enabledLayerCount = 0;
#endif

	if(vkCreateDevice(m_PhysicalDevice, &t_LogicalDeviceCreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	// create Graphics Queue, store the queue handles for later use
	vkGetDeviceQueue(m_LogicalDevice, t_QueueFamilies.m_GraphicsFamily.value(), 0, &a_GraphicsQueue);

	// create present queue, store the queue handle for later use
	vkGetDeviceQueue(m_LogicalDevice, t_QueueFamilies.m_PresentFamily.value(), 0, &a_PresentQueue);
}

// Note: might cause issues because it returns a copy, not a reference
VkPhysicalDevice Device::GetPhysicalDevice() const
{
	return m_PhysicalDevice;
}
