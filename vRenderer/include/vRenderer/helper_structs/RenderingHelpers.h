#pragma once
#include <optional>


struct SupportedQueueFamilies
{
	std::optional<uint32_t> m_GraphicsFamily;
	std::optional<uint32_t> m_PresentFamily;

	/// <summary>
	/// 	Determines if the graphics family and the present family contain any value.
	/// </summary>
	/// <returns>	True if it does, false if it does not. </returns>

	bool IsComplete() const
	{
		return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
	}
};

struct SwapChainInformation
{
	VkSurfaceCapabilitiesKHR m_SurfaceCapabilities = {};
	std::vector<VkSurfaceFormatKHR> m_SupportedSurfaceFormats = {};
	std::vector<VkPresentModeKHR> m_SupportedPresentModes = {};
};

struct InstanceCreateData
{
	VkApplicationInfo m_ApplicationInfo;
	VkInstanceCreateInfo m_CreateInfo;
};
