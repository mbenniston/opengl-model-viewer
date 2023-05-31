#pragma once
#include <glm_nowarn.h>
#include "../context/window.h"

class ICamera
{
public:
	virtual glm::mat4 getViewMatrix() = 0;
	virtual void update(Window& window, float delta) = 0;
	virtual void onGuiDraw() = 0;

	virtual ~ICamera() = default;
};