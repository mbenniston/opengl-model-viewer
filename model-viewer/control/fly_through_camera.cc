#include "fly_through_camera.h"
#include <iostream>

glm::mat4 FlyThroughCamera::getViewMatrix() 
{
	return m_viewMatrix;
}

void FlyThroughCamera::update(Window& window, float delta)
{
	if (m_firstFrame)
	{
		m_lastMousePosition = window.getMousePosition();
		m_firstFrame = false;
		return;
	}

	if (window.isKeyDown(GLFW_KEY_TAB))
	{
		m_firstFrame = true;
		m_enabled = true;
		return;
	}

	if (window.isKeyDown(GLFW_KEY_Q))
	{
		window.setCaptureMouse(false);
		m_enabled = false;
	}

	if (m_enabled)
	{
		window.setCaptureMouse(true);

		handleMovement(window, delta);
		handleLook(window, delta);
		updateViewMatrix();
	}
}

void FlyThroughCamera::onGuiDraw()
{
}

void FlyThroughCamera::handleMovement(Window& window, float delta)
{
	(void)delta;

	const float speed = 0.01f;

	if (window.isKeyDown(GLFW_KEY_A))
	{
		m_position -= m_right * speed;
	}

	if (window.isKeyDown(GLFW_KEY_D))
	{
		m_position += m_right * speed;
	}

	if (window.isKeyDown(GLFW_KEY_W))
	{
		m_position += m_forward * speed;
	}

	if (window.isKeyDown(GLFW_KEY_S))
	{
		m_position -= m_forward * speed;
	}

	if (window.isKeyDown(GLFW_KEY_SPACE))
	{
		m_position += m_up * speed;
	}

	if (window.isKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		m_position -= m_up * speed;
	}
}

void FlyThroughCamera::handleLook(Window& window, float delta)
{
	(void)delta;
	glm::vec2 currentMousePosition = window.getMousePosition();
	glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;
	m_lastMousePosition = currentMousePosition;

	const float sensitivity = 0.01f;

	m_yaw += mouseDelta.x * sensitivity;
	m_pitch -= mouseDelta.y * sensitivity;

	m_pitch = glm::clamp(m_pitch, -glm::radians(89.0f), glm::radians(89.0f));

	m_forward = glm::vec3(
		glm::cos(m_yaw) * glm::cos(m_pitch),
		glm::sin(m_pitch),
		glm::sin(m_yaw) * glm::cos(m_pitch)
	);

	m_right = glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f));
	m_up = glm::cross(m_right, m_forward);
}

void FlyThroughCamera::updateViewMatrix()
{
	m_viewMatrix = glm::lookAt(m_position, m_position + m_forward, m_up);
}

