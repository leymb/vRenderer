#pragma once
#include <glm/glm.hpp>
#include <array>
struct Vertex
{
	glm::vec2 m_Position;
	glm::vec3 m_Color;

	static VkVertexInputBindingDescription GenInputBindingDesc()
	{
		VkVertexInputBindingDescription t_Desc = {};

		t_Desc.binding = 0;
		t_Desc.stride = sizeof(Vertex);
		t_Desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return t_Desc;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GenInputAttributeDesc()
	{
		std::array<VkVertexInputAttributeDescription, 2> t_Desc = {};

		// desc for m_Position
		t_Desc[0].binding = 0;
		t_Desc[0].location = 0;
		t_Desc[0].format = VK_FORMAT_R32G32_SFLOAT;
		t_Desc[0].offset = offsetof(Vertex, m_Position);

		// desc for m_Color
		t_Desc[1].binding = 0;
		t_Desc[1].location = 1;
		t_Desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		t_Desc[1].offset = offsetof(Vertex, m_Color);

		return t_Desc;
	}
};