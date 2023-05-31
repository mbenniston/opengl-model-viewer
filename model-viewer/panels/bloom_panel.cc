#include <portable-file-dialogs.h>
#include <imgui.h>

#include "bloom_panel.h"

BloomPanel::BloomPanel(BloomSettings& settings) : m_settings(settings)
{
}

void BloomPanel::onDraw()
{
    ImGui::Begin("Bloom settings");
    ImGui::InputInt("Blur kernel", &m_settings.blurKernel);
    ImGui::InputFloat("Blur stddev", &m_settings.stdDev);
    ImGui::InputFloat("Factor", &m_settings.factor);
    ImGui::InputFloat("Lens dirt intensity", &m_settings.lensDirtIntensity);

	if (ImGui::Button("Load lens dirt texture"))
	{
		std::vector<std::string> selection = pfd::open_file("Select a file").result();
		if (!selection.empty())
		{
			const std::string& path = selection[0];
			try
			{
				m_settings.lensDirt = TextureWrapper::LoadFromPath(path);
			}
			catch (TextureLoadException loadException)
			{
				std::cout << "could not load texture from path: " << path << std::endl;
			}
		}
	}

	if (m_settings.lensDirt.has_value() && ImGui::Button("Clear lens dirt texture"))
	{
		m_settings.lensDirt.reset();
	}

    ImGui::End();
}
