#include <portable-file-dialogs.h>
#include "phong_panel.h"
#include <imgui.h>
#include "../renderers/pbr/ibl_baking.h"

PhongPanel::PhongPanel(PhongSettings& settings) : m_settings(settings)
{
}

void PhongPanel::onDraw()
{
	ImGui::Begin("Phong settings");
	ImGui::ColorEdit3("Ambient", glm::value_ptr(m_settings.ambient), ImGuiColorEditFlags_HDR);
	ImGui::InputFloat("Specular power", &m_settings.specularPower);
	ImGui::InputFloat("Specular strength", &m_settings.specularStrength);

	if (ImGui::InputInt("Source cubemap resolution", &m_settings.skyboxResolution, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		IblBaker iblBaker;
		m_settings.skyboxCubemap = iblBaker.convertEquirectangularToCubemap(m_settings.skyboxSource.value(), m_settings.skyboxResolution);
	}

	if (ImGui::Button("Load skybox"))
	{
		std::vector<std::string> selection = pfd::open_file("Select a file").result();
		if (!selection.empty())
		{
			const std::string& path = selection[0];
			IblBaker iblBaker;
			try
			{
				m_settings.skyboxSource = TextureWrapper::LoadHDRFromPath(path);
				m_settings.skyboxCubemap = iblBaker.convertEquirectangularToCubemap(m_settings.skyboxSource.value(), m_settings.skyboxResolution);
			}
			catch (TextureLoadException loadException)
			{
				std::cout << "could not load texture from path: " << path << std::endl;
			}
		}
	}

	if (m_settings.skyboxCubemap.has_value() && ImGui::Button("Clear skybox"))
	{
		m_settings.skyboxSource.reset();
		m_settings.skyboxCubemap.reset();
	}

	ImGui::End();
}
