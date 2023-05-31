#pragma once
#include "ipanel.h"
#include "../settings/gooch_settings.h"

class GoochPanel : public IPanel
{
public:
	GoochPanel(GoochSettings& settings);

	virtual void onDraw();

private:
	GoochSettings& m_settings;
};