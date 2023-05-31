#pragma once
#include "ipanel.h"
#include "../settings/ibl_settings.h"
#include "../renderers/pbr/ibl_baking.h"

class IblPanel : public IPanel
{
public:
	IblPanel(IblSettings& settings, IblBaker& iblBaker);

	virtual void onDraw();

private:
	IblSettings& m_settings;
	IblBaker& m_iblBaker;
};