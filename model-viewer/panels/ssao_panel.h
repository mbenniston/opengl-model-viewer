#pragma once
#include "ipanel.h"
#include "../settings/ssao_settings.h"

class SsaoPanel : public IPanel
{
public:
	SsaoPanel(SsaoSettings& settings);

	virtual void onDraw();

private:
	SsaoSettings& m_settings;
};