#pragma once
#include "vRenderer/Buffer/Buffer.h"
struct UniformBufferObject;

class UniformBuffer : public Buffer
{
public:
	UniformBuffer();
	~UniformBuffer();

	/// <summary>
	/// 	Creates a uniform buffer based on this library's UniformBufferObject Struct. Does not make use of a staging buffer because it is intended to
	/// 	be updated every frame.
	/// </summary>

	void CreateUniformBuffer(const Device& a_Device);

	/// <summary>	Creates a uniform buffer. This can be used to create a Uniform buffer with an arbitrary UBO. </summary>
	/// <param name="a_Device">  	The device.</param>
	/// <param name="a_BufferSize">	Size of the buffer.</param>

	void CreateUniformBuffer(const Device& a_Device, VkDeviceSize a_BufferSize);

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

	/// <summary>	
	///		Fills the Uniform Buffer using a pointer persistently mapped to the relating memory. 
	///		Allows for the use of an arbitrary UBO struct, not just the one used in this library. 
	///	</summary>
	/// <typeparam name="T">	Generic type parameter.</typeparam>
	/// <param name="a_Ubo">	The ubo.</param>

	template<typename T>
	void FillBuffer(T a_Ubo);

private:
	void* m_AccessPointer{};
};

template <typename T>
void UniformBuffer::FillBuffer(const T a_Ubo)
{
	memcpy(m_AccessPointer, &a_Ubo, sizeof(a_Ubo));
}

