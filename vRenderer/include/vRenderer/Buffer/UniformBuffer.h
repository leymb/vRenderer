#pragma once
#include "vRenderer/Buffer/Buffer.h"
struct UniformBufferObject;

class UniformBuffer : public Buffer
{
public:
	UniformBuffer();
	~UniformBuffer();

	/// <summary>
	/// 	Creates a uniform buffer. Does not make use of a staging buffer because it is intended to
	/// 	be updated every frame.
	/// </summary>

	void CreateUniformBuffer(const Device& a_Device);

	/// <summary>	Creates a descriptor set layout for a Uniform Buffer. </summary>
	/// <exception cref="std::runtime_error">	Raised when the DescriptorSetLayout could not be
	/// 										created.</exception>
	/// <param name="a_Device">	A logical device.</param>
	/// <returns>	The new descriptor set layout. </returns>

	static VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice a_Device);

	/// <summary>
	/// 	Fills the Uniform Buffer using a pointer persistently mapped to the relating memory.
	/// </summary>
	/// <param name="a_Ubo">	[in,out] The Uniform Buffer Object.</param>

	void FillBuffer(const UniformBufferObject& a_Ubo);

private:
	void* m_AccessPointer{};
};

