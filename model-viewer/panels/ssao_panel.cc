#include "ssao_panel.h"
#include <imgui.h>

SsaoPanel::SsaoPanel(SsaoSettings& settings) : m_settings(settings)
{
}

void SsaoPanel::onDraw()
{
    ImGui::Begin("SSAO Settings");
    ImGui::Checkbox("Enabled", &m_settings.enabled);
    ImGui::InputInt("Samples", &m_settings.samples);
    ImGui::InputFloat("Distance", &m_settings.distance);
    ImGui::InputFloat("Bias", &m_settings.bias);
    ImGui::InputInt("Blur kernel", &m_settings.blurKernel);
    ImGui::End();
}


