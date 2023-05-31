#include <portable-file-dialogs.h>
#include "scene_panel.h"
#include <imgui.h>

#include "../renderers/pbr/pbr_renderer.h"
#include "../renderers/gooch/gooch_renderer.h"
#include "../renderers/phong/phong_renderer.h"

ScenePanel::ScenePanel(SceneSettings& settings, std::unique_ptr<IRenderer>& renderer) : 
    m_settings(settings),
    m_renderer(renderer)
{
}

void ScenePanel::onDraw()
{
    ImGui::Begin("Scene settings");

    const std::string& path_name = m_settings.loadedModelPath.value_or("no model loaded");
    ImGui::Text(path_name.c_str());

    if (ImGui::Button("Load glTF model"))
    {
        std::vector<std::string> selection = pfd::open_file("Select a file").result();
        if (!selection.empty())
        {
            const std::string& path = selection[0];
            m_settings.loadModel(path);
        }
    }

    static const char* renderModeStrings = "Gooch\0Phong\0PBR\0";

    int mode = static_cast<int>(m_settings.renderMode);
    if (ImGui::Combo("Render mode", &mode, renderModeStrings))
    {
        m_settings.renderMode = static_cast<SceneRenderMode>(mode);
        m_renderer.reset();

        switch (m_settings.renderMode)
        {
        case SceneRenderMode::Gooch:
            m_renderer = std::make_unique<GoochRenderer>(m_settings);
            break;
        case SceneRenderMode::Phong:
            m_renderer = std::make_unique<PhongRenderer>(m_settings);
            break;
        case SceneRenderMode::PBR:
            m_renderer = std::make_unique<PbrRenderer>(m_settings);
            break;
        }
    }

    static const char* cameraModeStrings = "Orbit\0First person\0";

    int cameraMode = static_cast<int>(m_settings.cameraMode);
    ImGui::Combo("Camera mode", &cameraMode, cameraModeStrings);
    m_settings.cameraMode = static_cast<SceneCameraMode>(cameraMode);

    ImGui::DragFloat3("light direction", glm::value_ptr(m_settings.lightDirection), 0.01f, -1.0f, 1.0f);
    ImGui::ColorEdit3("Light radiance", glm::value_ptr(m_settings.lightColor), ImGuiColorEditFlags_HDR);
    ImGui::Checkbox("Enable debug normals", &m_settings.debugNormals);

    ImGui::End();
}
