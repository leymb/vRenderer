#include "pch.h"
#include "vRenderer/camera/Camera.h"
#include "vRenderer/camera/Camera.h"
#include "vRenderer/camera/Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(const glm::vec3& a_Position, const int a_WindowWidth, const int a_WindowHeight) :
	m_Position(a_Position),
	m_Up(glm::vec3(0.0f, 0.0f, 1.0f)),
	m_FOV(45.0f),
	m_Aspect(static_cast<float>(a_WindowWidth) / static_cast<float>(a_WindowHeight))
{
	m_View = lookAt(a_Position, glm::vec3(0.f, 0.f, 0.f), m_Up);
}

Camera::Camera(const glm::vec3& a_Position, const glm::vec3& a_LookAt, const int a_WindowWidth, const int a_WindowHeight) :
	Camera(a_Position, a_WindowWidth, a_WindowHeight)
{
	m_View = lookAt(a_Position, a_LookAt, m_Up);
}

Camera::~Camera()
= default;

void Camera::UpdateViewMat(const glm::vec3& a_LookAt)
{
	m_View = lookAt(m_Position, a_LookAt, m_Up);
}

// Pos glm::vec3(2.0f, 2.0f, 2.0f)
// lookat glm::vec3(0.0f, 0.0f, 0.0f)
glm::mat4 Camera::GetViewMat() const
{
	return m_View;
}

glm::mat4 Camera::GetProjectionMat() const
{
	return glm::perspective(glm::radians(m_FOV), m_Aspect, m_Near, m_Far);
}

glm::vec3 Camera::GetPosition() const
{
	return m_Position;
}

void Camera::SetPosition(const glm::vec3& a_Position)
{
	m_Position = a_Position;
}

void Camera::UpdateAspectRatio(const uint32_t a_WindowWidth, const uint32_t a_WindowHeight)
{
	m_Aspect = static_cast<float>(a_WindowWidth) / static_cast<float>(a_WindowHeight);
}

void Camera::UpdateAspectRatio(const glm::ivec2& a_WindowExtent)
{
	UpdateAspectRatio(a_WindowExtent.x, a_WindowExtent.y);
}

void Camera::UpdateFOV(float a_FOV)
{
	m_FOV = a_FOV;
}
