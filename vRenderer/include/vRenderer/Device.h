#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

class SwapChain;
class Device
{
public:
	Device();

	/// <summary>	Selects a physical device. </summary>
	/// <param name="a_Instance">				  	The Vulkan instance.</param>
	/// <param name="a_Surface">				  	The window surface.</param>
	/// <param name="a_RequestedDeviceExtensions">	The requested device extensions.</param>

	VkPhysicalDevice ChoosePhysicalDevice(VkInstance& a_Instance, VkSurfaceKHR a_Surface, const std::vector<const char*>& a_RequestedDeviceExtensions);

	/// <summary>
	/// 	Creates a logical device. Requires that a physical device has been selected first.
	/// </summary>
	/// <param name="a_Surface">				  	The surface.</param>
	/// <param name="a_GraphicsQueue">			  	Queue of graphics.</param>
	/// <param name="a_PresentQueue">			  	Queue of presents.</param>
	/// <param name="a_RequestedDeviceExtensions">	The requested device extensions.</param>
	/// <param name="a_EnabledValidationLayers">  	The enabled validation layers.</param>

	void CreateLogicalDevice(VkSurfaceKHR a_Surface, VkQueue& a_GraphicsQueue, VkQueue& a_PresentQueue,
	                         const std::vector<const char*>& a_RequestedDeviceExtensions,
	                         const std::vector<const char*>& a_EnabledValidationLayers);

	VkPhysicalDevice GetPhysicalDevice() const;
	VkDevice GetLogicalDevice() const;

	/// <summary>	Gets maximum MSAA samples the physical device supports. </summary>
	/// <returns>	The maximum MSAA sample count. </returns>

	VkSampleCountFlagBits GetMSAASampleCount() const;

private:

	bool CheckDeviceSuitability(VkPhysicalDevice a_Device, VkSurfaceKHR a_Surface, const std::vector<const char*>& a_RequestedDeviceExtensions) const;

	static bool CheckSwapChainCompatibility(const VkPhysicalDevice& a_Device, const VkSurfaceKHR& a_WindowSurface);

	bool CheckDeviceExtensionSupport(VkPhysicalDevice a_Device, const std::vector<const char*>& a_RequestedDeviceExtensions) const;

	VkSampleCountFlagBits CheckMSAASampleCount(VkPhysicalDevice& a_PhysicalDevice);

	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_LogicalDevice;

	VkSampleCountFlagBits m_MSAASampleCount;
};

