#pragma once
#include "ipanel.h"
#include "../settings/phong_settings.h"

class PhongPanel : public IPanel
{
public:
	PhongPanel(PhongSettings& settings);

	virtual void onDraw();

private:
	PhongSettings& m_settings;
};