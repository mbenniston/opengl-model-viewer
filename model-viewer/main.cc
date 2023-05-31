#include <iostream>
#include <fstream>
#include <map>

#include <imgui.h>
#include "constants.h"
#include "context/window.h"

#include "panels/scene_panel.h"
#include "renderers/pbr/pbr_renderer.h"

int main()
{
    std::unique_ptr<IRenderer> renderer;
    Window window(1920, 1080, [&renderer](int new_width, int new_height) { renderer->onWindowResize(new_width, new_height); });

    float fov = 70.0f;

    SceneSettings sceneSettings;
    sceneSettings.width = 1920;
    sceneSettings.height = 1080;
    sceneSettings.loadModel(RESOURCES_PATH + "models/DamagedHelmet.glb");
 
    ScenePanel scene_panel(sceneSettings, renderer);

    renderer = std::make_unique<PbrRenderer>(sceneSettings);

    float lastFrameTime = 0.0f;
    float delta = 0.0f;

    bool showGui = true;
    bool wasToggleShowGuiButtonPressed = false;

    while (!window.isOpen() && !window.isKeyDown(GLFW_KEY_ESCAPE))
    {
        sceneSettings.width = window.getWidth();
        sceneSettings.height = window.getHeight();

        ICamera& camera = sceneSettings.getCamera();
        camera.update(window, delta);

        glm::mat4 projectionMatrix = glm::perspectiveFov(glm::radians(fov), (float)window.getWidth(), (float)window.getHeight(), 0.1f, 100.0f);
        glm::mat4 viewMatrix = camera.getViewMatrix();

        renderer->onRender(projectionMatrix, viewMatrix);
        
        bool toggleShowGuiButtonPressed = window.isKeyDown(GLFW_KEY_E);

        if (!wasToggleShowGuiButtonPressed && toggleShowGuiButtonPressed)
        {
            showGui = !showGui;
            wasToggleShowGuiButtonPressed = true;
        }

        if (wasToggleShowGuiButtonPressed && !toggleShowGuiButtonPressed)
        {
            wasToggleShowGuiButtonPressed = false;
        }

        window.startImGui();

        if (showGui)
        {
            scene_panel.onDraw();
            renderer->onGuiDraw();
            camera.onGuiDraw();

            ImGui::Begin("Frametimes");
            ImGui::Text("Frametime: %f ms", delta * 1000);
            ImGui::End();
        }

        window.endImGui();

        window.swapBuffers();

        float current_time = static_cast<float>(glfwGetTime());
        delta = current_time - lastFrameTime;
        lastFrameTime = current_time;
    }
    renderer.reset();

    return 0;
}