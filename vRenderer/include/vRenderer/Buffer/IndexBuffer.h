#pragma once
#include <vector>
#include "vRenderer/Buffer/Buffer.h"

class IndexBuffer : public Buffer
{
public:
	IndexBuffer();
	~IndexBuffer();

	void CreateIndexBuffer(std::vector<uint16_t> a_Indices, const Device& a_Device, VkQueue a_GraphicsQueue,
	              VkCommandPool a_CommandPool);

private:
	std::vector<uint16_t> m_Indices{};
};

