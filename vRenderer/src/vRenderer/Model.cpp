#include "pch.h"
#include "vRenderer/Model.h"

#include <unordered_map>
#include <glm/ext/matrix_transform.hpp>
#include <tiny_obj/tiny_obj_loader.h>

Model::Model() : m_Position(glm::vec3(0.f)), m_Scale(1.0f), m_Rotation(glm::mat4(0.f))
{
	
}

Model::~Model()
= default;

void Model::Load(const char* a_ModelPath, const char* a_TexturePath, const Device& a_Device,
                 VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue)
{
	// load texture
	m_Texture.CreateTextureFromImage(a_TexturePath, a_Device, a_CommandPool, a_GraphicsQueue);
	m_Texture.CreateTextureSampler(a_Device);

	// load mesh
	LoadMesh(a_ModelPath);
}

void Model::CreateFromMesh(Mesh& a_Mesh, const char* a_TexturePath, const Device& a_Device,
                 VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue)
{
	m_Mesh = a_Mesh;

	// load texture
	m_Texture.CreateTextureFromImage(a_TexturePath, a_Device, a_CommandPool, a_GraphicsQueue);
	m_Texture.CreateTextureSampler(a_Device);
}

void Model::Destroy(VkDevice a_LogicalDevice)
{
	m_Texture.DestroyTexture(a_LogicalDevice);
}

const Texture& Model::GetTexture()
{
	return m_Texture;
}

Mesh& Model::GetMesh()
{
	return m_Mesh;
}

void Model::Rotate(float a_Angle, glm::vec3 a_Axis)
{
	m_Rotation = glm::rotate(glm::mat4(1.0f), glm::radians(a_Angle), a_Axis);
}

glm::mat4 Model::GetRotation()
{
	return m_Rotation;
}

void Model::SetScale(float a_Scale)
{
	m_Scale = a_Scale;
}

glm::mat4 Model::GetScale()
{
	return glm::scale(glm::mat4(1.0f), glm::vec3(m_Scale));
}

void Model::SetPosition(glm::vec3 a_Pos)
{
	m_Position = a_Pos;
}

glm::mat4 Model::GetTranslation()
{
	return glm::translate(glm::mat4(1.0f), m_Position);
}

glm::mat4 Model::GetModelMatrix()
{
	//sinf(t_Delta) * 0.5f
	// return GetTranslation() * GetRotation() * GetScale();
	return GetScale() * GetRotation() * GetTranslation();
}

void Model::LoadMesh(const char* a_ModelPath)
{
	// load faces using tinyobj
	tinyobj::attrib_t t_Attrib;
	std::vector<tinyobj::shape_t> t_Shapes;
	std::vector<tinyobj::material_t> t_Materials;

	std::string t_Warning;
	std::string t_Error;

	if (!LoadObj(&t_Attrib, &t_Shapes, &t_Materials, &t_Warning, &t_Error, a_ModelPath))
	{
		throw std::runtime_error(t_Warning + t_Error);
	}

	// combine all faces
	std::unordered_map<Vertex, uint32_t> t_UniqueVertices{};
	for (const tinyobj::shape_t& t_Shape : t_Shapes)
	{

		for (const auto& t_Index : t_Shape.mesh.indices)
		{
			Vertex t_Vertx = {};

			t_Vertx.m_Position = {
				t_Attrib.vertices[3 * t_Index.vertex_index + 0],
				t_Attrib.vertices[3 * t_Index.vertex_index + 1],
				t_Attrib.vertices[3 * t_Index.vertex_index + 2]
			};

			t_Vertx.m_TexCoord = {
				t_Attrib.texcoords[2 * t_Index.texcoord_index + 0],
				// compensate for obj coordinate system alignment 
				1.0f - t_Attrib.texcoords[2 * t_Index.texcoord_index + 1]
			};

			if (t_UniqueVertices.count(t_Vertx) == 0)
			{
				t_UniqueVertices[t_Vertx] = static_cast<uint32_t>(m_Mesh.m_Vertices.size());
				m_Mesh.m_Vertices.push_back(t_Vertx);
			}

			m_Mesh.m_Indices.push_back(t_UniqueVertices[t_Vertx]);
		}
	}
}
