#include <portable-file-dialogs.h>
#include "ibl_panel.h"
#include <imgui.h>

IblPanel::IblPanel(IblSettings& settings, IblBaker& iblBaker) : m_settings(settings), m_iblBaker(iblBaker)
{
}

void IblPanel::onDraw()
{
    ImGui::Begin("IBL Settings");

    if (ImGui::InputInt("Source cubemap resolution", &m_settings.sourceCubemapSize, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_settings.diffuseCubemap = m_iblBaker.convertEquirectangularToCubemap(m_settings.equirectangularEnvironment, m_settings.sourceCubemapSize);
        m_settings.irrandianceCubemap = m_iblBaker.convoluteCubemap(m_settings.diffuseCubemap, m_settings.irrandianceCubemapSize);
        m_settings.filteredCubemap = m_iblBaker.prefilterCubemap(m_settings.diffuseCubemap, m_settings.filteredCubemapSize, 5);
    }

    if (ImGui::InputInt("Irradiance cubemap resolution", &m_settings.irrandianceCubemapSize, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_settings.irrandianceCubemap = m_iblBaker.convoluteCubemap(m_settings.diffuseCubemap, m_settings.irrandianceCubemapSize);
        m_settings.filteredCubemap = m_iblBaker.prefilterCubemap(m_settings.diffuseCubemap, m_settings.filteredCubemapSize, 5);
    }

    if (ImGui::InputInt("Filtered cubemap resolution", &m_settings.filteredCubemapSize, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_settings.filteredCubemap = m_iblBaker.prefilterCubemap(m_settings.diffuseCubemap, m_settings.filteredCubemapSize, 5);
    }

    if (ImGui::InputInt("IBL BRDF lut resolution", &m_settings.brdfLutSize, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_settings.brdfLut = m_iblBaker.generateBrdfLut(m_settings.brdfLutSize);
    }

    if (ImGui::Button("Load environment map"))
    {
        std::vector<std::string> selection = pfd::open_file("Select a file").result();
        if (!selection.empty())
        {
            const std::string& path = selection[0];

            try {
                m_settings.equirectangularEnvironment = TextureWrapper::LoadHDRFromPath(path);
                m_settings.diffuseCubemap = m_iblBaker.convertEquirectangularToCubemap(m_settings.equirectangularEnvironment, m_settings.sourceCubemapSize);
                m_settings.irrandianceCubemap = m_iblBaker.convoluteCubemap(m_settings.diffuseCubemap, m_settings.irrandianceCubemapSize);
                m_settings.filteredCubemap = m_iblBaker.prefilterCubemap(m_settings.diffuseCubemap, m_settings.filteredCubemapSize, 5);
            }
            catch (TextureLoadException loadException)
            {
                std::cout << "could not load texture from path: " << path << std::endl;
            }
        }
    }

    ImGui::End();
}


