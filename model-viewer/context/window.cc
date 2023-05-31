#include "window.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

static void resize_callback_glfw(GLFWwindow* window, int width, int height)
{
	std::function<void(int, int)> callback = *(std::function<void(int, int)>*)glfwGetWindowUserPointer(window);
	callback(width, height);
}

Window::Window(int width, int height, std::function<void(int,int)> resizeCallback) : 
	m_resizeCallback(resizeCallback)
{	
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_handle = glfwCreateWindow(width, height, "opengl-renderer", NULL, NULL);
	glfwMakeContextCurrent(m_handle);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetWindowUserPointer(m_handle, &m_resizeCallback);
	glfwSetWindowSizeCallback(m_handle, resize_callback_glfw);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; 

	ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForOpenGL(m_handle, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

int Window::getWidth() const
{
	int width;
	glfwGetWindowSize(m_handle, &width, nullptr);
	return width;
}

int Window::getHeight() const
{
	int height;
	glfwGetWindowSize(m_handle, nullptr, &height);
	return height;
}

bool Window::isOpen() const
{
	return glfwWindowShouldClose(m_handle);
}

void Window::startImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::endImGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::swapBuffers()
{
	glfwSwapBuffers(m_handle);
	glfwPollEvents();
}

void Window::setCaptureMouse(bool shouldCaptureMouse)
{
	glfwSetInputMode(m_handle, GLFW_CURSOR, shouldCaptureMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool Window::isKeyDown(int keyCode) const
{
	return glfwGetKey(m_handle, keyCode) == GLFW_PRESS;
}

glm::vec2 Window::getMousePosition() const
{
	double mx, my;
	glfwGetCursorPos(m_handle, &mx, &my);
	return glm::vec2(mx, my);
}

bool Window::isMouseButtonDown(int button) const
{
	return glfwGetMouseButton(m_handle, button) == GLFW_PRESS;
}
