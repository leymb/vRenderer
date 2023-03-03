#pragma once
#include "vRenderer/Buffer/Buffer.h"
#include <vector>

class Device;
struct Vertex;

class VertexBuffer : public Buffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	/// <summary>	Creates a vertex buffer and allocates memory for it. </summary>
	/// <param name="a_Vertices">	  	[in,out] The vertices.</param>
	/// <param name="a_Device">		  	The logical device.</param>
	/// <param name="a_GraphicsQueue">	Queue used to execute the copy command.</param>
	/// <param name="a_CommandPool">  	The command pool that should execute the transfer commands.</param>

	void CreateVertexBuffer(std::vector<Vertex>& a_Vertices, const Device& a_Device, VkQueue a_GraphicsQueue,
	              VkCommandPool a_CommandPool);
};