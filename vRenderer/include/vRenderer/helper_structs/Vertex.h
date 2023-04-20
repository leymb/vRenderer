#pragma once
#include <glm/glm.hpp>
#include <array>
struct Vertex
{
	glm::vec3 m_Position;
	glm::vec3 m_Color = {1.0f, 0.0f, 1.0f};
	glm::vec2 m_TexCoord;

	static VkVertexInputBindingDescription GenInputBindingDesc()
	{
		VkVertexInputBindingDescription t_Desc = {};

		t_Desc.binding = 0;
		t_Desc.stride = sizeof(Vertex);
		t_Desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return t_Desc;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GenInputAttributeDesc()
	{
		std::array<VkVertexInputAttributeDescription, 3> t_Desc = {};

		// desc for m_Position
		t_Desc[0].binding = 0;
		t_Desc[0].location = 0;
		t_Desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		t_Desc[0].offset = offsetof(Vertex, m_Position);

		// desc for m_Color
		t_Desc[1].binding = 0;
		t_Desc[1].location = 1;
		t_Desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		t_Desc[1].offset = offsetof(Vertex, m_Color);

		// texture coordinates
		t_Desc[2].binding = 0;
		t_Desc[2].location = 2;
		t_Desc[2].format = VK_FORMAT_R32G32_SFLOAT;
		t_Desc[2].offset = offsetof(Vertex, m_TexCoord);

		return t_Desc;
	}

	bool operator==(const Vertex& a_Other) const
	{
		return	m_Position == a_Other.m_Position		&&
				m_Color == a_Other.m_Color			&&
				m_TexCoord == a_Other.m_TexCoord;
	}
};

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& a_Vertex) const noexcept
		{
			return ((hash<glm::vec3>()(a_Vertex.m_Position) ^
                   (hash<glm::vec3>()(a_Vertex.m_Color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(a_Vertex.m_TexCoord) << 1);
		}
	};
}
