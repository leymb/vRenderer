#pragma once
#include "vRenderer/Buffer.h"
#include <vector>

class Device;
struct Vertex;

class VertexBuffer : public Buffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	/// <summary>	Creates a vertex buffer and allocates memory for it. </summary>
	/// <param name="a_Vertices">	[in,out] The vertices.</param>
	/// <param name="a_Device">  	The logical device.</param>

	void CreateVertexBuffer(std::vector<Vertex>& a_Vertices, const Device& a_Device);
};