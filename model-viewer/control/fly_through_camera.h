#pragma once
#include "icamera.h"

class FlyThroughCamera : public ICamera
{
public:
	virtual glm::mat4 getViewMatrix();
	virtual void update(Window& window, float delta);
	virtual void onGuiDraw();

private:
	void handleMovement(Window& window, float delta);
	void handleLook(Window& window, float delta);
	void updateViewMatrix();

	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;

	glm::mat4 m_viewMatrix;
	glm::vec2 m_lastMousePosition;
	bool m_firstFrame = true;
	bool m_enabled = true;
};