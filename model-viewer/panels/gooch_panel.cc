#include "gooch_panel.h"
#include <imgui.h>

GoochPanel::GoochPanel(GoochSettings& settings) : m_settings(settings)
{
}

void GoochPanel::onDraw()
{
    ImGui::Begin("Gooch settings");
    ImGui::InputFloat("Blue", &m_settings.blue);
    ImGui::InputFloat("Yellow", &m_settings.yellow);
    ImGui::InputFloat("Alpha", &m_settings.alpha);
    ImGui::InputFloat("Beta", &m_settings.beta);
    ImGui::InputInt("Scale", &m_settings.scale);
    ImGui::InputFloat("Depth threshold", &m_settings.depthThreshold);
    ImGui::InputFloat("Normal threshold", &m_settings.normalThreshold);
    ImGui::InputFloat("Depth normal threshold", &m_settings.depthNormalThreshold);
    ImGui::InputFloat("Depth normal threshold scale", &m_settings.depthNormalThresholdScale);
    ImGui::End();
}
