#pragma once
#include "vRenderer/Texture.h"
#include "vRenderer/helper_structs/Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION

class Model
{
public:
	Model();
	~Model();

	/// <summary>	Loads an obj file and texture from the specified paths. </summary>
	/// <param name="a_ModelPath">	  	Full pathname of the model file.</param>
	/// <param name="a_TexturePath">  	Full pathname of the texture file.</param>
	/// <param name="a_Device">		  	The device.</param>
	/// <param name="a_CommandPool">  	[in,out] The command pool.</param>
	/// <param name="a_GraphicsQueue">	Graphics Queue.</param>

	void Load(const char* a_ModelPath, const char* a_TexturePath, const Device& a_Device, VkCommandPool& a_CommandPool,
	          const VkQueue& a_GraphicsQueue);

	void CreateFromMesh(Mesh& a_Mesh, const char* a_TexturePath, const Device& a_Device,
                 VkCommandPool& a_CommandPool, const VkQueue& a_GraphicsQueue);

	void Destroy(VkDevice a_LogicalDevice);

	const Texture& GetTexture();
	Mesh& GetMesh();

	void Rotate(float a_Angle, glm::vec3 a_Axis);
	glm::mat4 GetRotation();

	void SetScale(float a_Scale);
	glm::mat4 GetScale();

	void SetPosition(glm::vec3 a_Pos);
	glm::mat4 GetTranslation();

	glm::mat4 GetModelMatrix();

private:
	void LoadMesh(const char* a_ModelPath);

	Mesh m_Mesh;
	Texture m_Texture;

	glm::vec3 m_Position{};
	float m_Scale;
	glm::mat4 m_Rotation{};
};

