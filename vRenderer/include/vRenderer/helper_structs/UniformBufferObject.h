#pragma once
#include <glm/glm.hpp>

struct UniformBufferObject
{
	// explicit memory alignment for members
	alignas(16) glm::mat4 m_Model = {};
	alignas(16) glm::mat4 m_View = {};
	alignas(16) glm::mat4 m_Projection = {};
};
