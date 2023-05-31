#pragma once

#include "../irenderer.h"
#include "../../settings/scene_settings.h"
#include "phong_pass.h"
#include "../../settings/phong_settings.h"
#include "../../panels/phong_panel.h"
#include "../pbr/ibl_baking.h"

class PhongRenderer : public IRenderer
{
public:
	PhongRenderer(SceneSettings& scene_settings);

	virtual void onRender(const glm::mat4& projectionMatrix, glm::mat4& viewMatrix);
	virtual void onGuiDraw();
	virtual void onWindowResize(int newWidth, int newWeight);

private:
	SceneSettings& m_sceneSettings;
	PhongSettings m_settings;
	PhongPanel m_panel;

	PhongPass m_phongPass;
};