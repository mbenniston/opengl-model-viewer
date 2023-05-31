#pragma once
#include "icamera.h"

class OrbitCamera : public ICamera
{
public:
	virtual glm::mat4 getViewMatrix();
	virtual void update(Window& window, float delta);
	virtual void onGuiDraw();

private:
	void handleLook(Window& window, float delta);
	void updateViewMatrix();

	glm::vec3 m_focus = glm::vec3(0.0f);
	float m_distance = 1.0f;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	float m_panSensitivity = 0.1f;
	float m_distanceSensitivity = 0.01f;
	float m_grabSensitivity = 0.005f;
	float m_spinSpeed = 6.0f;

	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 m_viewMatrix;
	glm::vec2 m_lastMousePosition;
	bool m_firstFrame = true;
};