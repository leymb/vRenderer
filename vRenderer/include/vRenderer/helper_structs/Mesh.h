#pragma once
#include <vector>
#include "vRenderer/helper_structs/Vertex.h"
struct Mesh
{
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
};

