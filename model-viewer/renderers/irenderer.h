#pragma once
#include <glm_nowarn.h>

class IRenderer
{
public:
	virtual void onRender(const glm::mat4& projectionMatrix, glm::mat4& viewMatrix) = 0;
	virtual void onGuiDraw() = 0;
	virtual void onWindowResize(int newWidth, int newHeight) = 0;
	virtual ~IRenderer() = default;
};