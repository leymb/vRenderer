#pragma once
struct Particle
{
	glm::vec2 m_Pos;
	glm::vec2 m_Velocity;
	glm::vec4 m_Color;

	static VkVertexInputBindingDescription GenBindingDescription()
	{
		VkVertexInputBindingDescription t_Description = {};
		t_Description.binding = 0;
		t_Description.stride = sizeof(Particle);
		t_Description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return t_Description;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GenAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> t_Descriptions = {};

		t_Descriptions[0].binding = 0;
		t_Descriptions[0].location = 0;
		t_Descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		t_Descriptions[0].offset = offsetof(Particle, m_Pos);

		t_Descriptions[1].binding = 0;
		t_Descriptions[1].location = 1;
		t_Descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		t_Descriptions[1].offset = offsetof(Particle, m_Color);

		return t_Descriptions;
	}
};
