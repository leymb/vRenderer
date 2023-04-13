#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera(const glm::vec3& a_Position, int a_WindowWidth, int a_WindowHeight);
	Camera(const glm::vec3& a_Position, const glm::vec3& a_LookAt, int a_WindowWidth, int a_WindowHeight);

	~Camera();

	void UpdateViewMat(const glm::vec3& a_LookAt);
	glm::mat4 GetViewMat() const;
	glm::mat4 GetProjectionMat() const;

	glm::vec3 GetPosition() const;

	void SetPosition(const glm::vec3& a_Position);

	/// <summary>	Updates the aspect ratio. </summary>
	/// <param name="a_WindowWidth"> 	Width of the window.</param>
	/// <param name="a_WindowHeight">	Height of the window.</param>

	void UpdateAspectRatio(uint32_t a_WindowWidth, uint32_t a_WindowHeight);

	/// <summary>	Updates the aspect ratio described by a_WindowExtent. </summary>
	/// <param name="a_WindowExtent">	Extent of the window, in the format of width, height.</param>

	void UpdateAspectRatio(const glm::ivec2& a_WindowExtent);

	void UpdateFOV(float a_FOV);

private:
	glm::vec3 m_Position;
	glm::vec3 m_Up;

	float m_Near = 0.1f;
	float m_Far = 1000.f;

	float m_FOV;
	float m_Aspect;

	glm::mat4 m_View;
};

