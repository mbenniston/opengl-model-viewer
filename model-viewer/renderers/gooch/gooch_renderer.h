#pragma once
#include "../irenderer.h"
#include "gooch_pass.h"
#include "../../settings/scene_settings.h"
#include "../../settings/gooch_settings.h"
#include "../../panels/gooch_panel.h"

class GoochRenderer : public IRenderer
{
public:
	GoochRenderer(SceneSettings& scene_settings);

	virtual void onRender(const glm::mat4& projectionMatrix, glm::mat4& viewMatrix);
	virtual void onGuiDraw();
	virtual void onWindowResize(int newWidth, int newHeight);

private:
	SceneSettings& m_sceneSettings;
	GoochPass m_goochPass;
	GoochSettings m_goochSettings;
	GoochPanel m_goochPanel;
};