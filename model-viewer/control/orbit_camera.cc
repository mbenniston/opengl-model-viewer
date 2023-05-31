#include "orbit_camera.h"
#include <imgui.h>

glm::mat4 OrbitCamera::getViewMatrix()
{
	return m_viewMatrix;
}

void OrbitCamera::update(Window& window, float delta)
{
	if (m_firstFrame)
	{
		m_lastMousePosition = window.getMousePosition();
		m_firstFrame = false;
		return;
	}

	if (window.isKeyDown(GLFW_KEY_R))
	{
		m_yaw += m_spinSpeed * delta;
	}

	handleLook(window, delta);
	updateViewMatrix();
}

void OrbitCamera::onGuiDraw()
{
	ImGui::Begin("Orbit camera settings");
	ImGui::InputFloat3("Focus", glm::value_ptr(m_focus));
	ImGui::InputFloat("Distance", &m_distance);
	ImGui::InputFloat("Pitch", &m_pitch);
	ImGui::InputFloat("Yaw", &m_yaw);

	ImGui::InputFloat("Pan sensitivity", &m_panSensitivity);
	ImGui::InputFloat("Distance sensitivity", &m_distanceSensitivity);
	ImGui::InputFloat("Grab sensitivity", &m_grabSensitivity);
	ImGui::InputFloat("Spin speed", &m_spinSpeed);

	if (ImGui::Button("Reset"))
	{
		m_focus = glm::vec3(0.0f);
		m_position = glm::vec3(0.0f);
		m_distance = 1.0f;

		m_right = glm::vec3(1.0f, 0.0f, 0.0f);
		m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
		m_up = glm::vec3(0.0f, 1.0f, 0.0f);

		m_panSensitivity = 0.1f;
		m_distanceSensitivity = 0.01f;
		m_grabSensitivity = 0.005f;

		m_yaw = 0.0f;
		m_pitch = 0.0f;
	}

	ImGui::End();
}

void OrbitCamera::handleLook(Window& window, float delta)
{
	(void)delta;
	glm::vec2 currentMousePosition = window.getMousePosition();
	glm::vec2 mouseDelta = currentMousePosition - m_lastMousePosition;
	m_lastMousePosition = currentMousePosition;

	bool grabbingInput = false;

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureKeyboard || io.WantCaptureMouse)
	{
		return;
	}

	if (window.isMouseButtonDown(2))
	{
		m_focus += (m_right * -mouseDelta.x + m_up * mouseDelta.y) * m_grabSensitivity * std::sqrt(m_distance);
		grabbingInput = true;
	}
	else if (window.isMouseButtonDown(0))
	{
		m_pitch += mouseDelta.y * m_panSensitivity;
		m_yaw += mouseDelta.x * m_panSensitivity;
		grabbingInput = true;
	}
	else if (window.isMouseButtonDown(1))
	{
		m_distance += mouseDelta.y * std::sqrt(m_distance) * m_distanceSensitivity;
		grabbingInput = true;
	}

	ImGui::CaptureKeyboardFromApp(!grabbingInput);
	ImGui::CaptureMouseFromApp(!grabbingInput);
}

void OrbitCamera::updateViewMatrix()
{
	m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
	m_distance = glm::max(0.01f, m_distance);

	const float pitch_radians = glm::radians(m_pitch);
	const float yaw_radians = glm::radians(m_yaw);

	m_position = m_focus + m_distance * glm::vec3(
		glm::cos(yaw_radians) * glm::cos(pitch_radians),
		glm::sin(pitch_radians),
		glm::sin(yaw_radians) * glm::cos(pitch_radians)
	);

	m_forward = glm::normalize(m_focus - m_position);
	m_right = glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f));
	m_up = glm::cross(m_right, m_forward);

	m_viewMatrix = glm::lookAt(m_position, m_position + m_forward, m_up);
}
