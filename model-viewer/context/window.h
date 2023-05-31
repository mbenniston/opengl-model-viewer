#pragma once
#include <functional>
#include <glm_nowarn.h>
#include <glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(int width, int height, std::function<void(int, int)> resizeCallback);
	~Window();
	
	int getWidth() const;
	int getHeight() const;

	bool isOpen() const;

	void startImGui();
	void endImGui();
	void swapBuffers();

	void setCaptureMouse(bool shouldCaptureMouse);

	bool isKeyDown(int keyCode) const;
	glm::vec2 getMousePosition() const;

	bool isMouseButtonDown(int button) const;

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&& other) = delete;
	Window& operator=(Window&& other) = delete;

private:
	GLFWwindow* m_handle;
	std::function<void(int, int)> m_resizeCallback;
};