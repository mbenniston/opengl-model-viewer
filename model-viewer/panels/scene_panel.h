#pragma once
#include "ipanel.h"
#include "../settings/scene_settings.h"
#include "../renderers/irenderer.h"

class ScenePanel : public IPanel
{
public:
	ScenePanel(SceneSettings& settings, std::unique_ptr<IRenderer>& renderer);

	virtual void onDraw();

private:
	SceneSettings& m_settings;

	std::unique_ptr<IRenderer>& m_renderer;
};