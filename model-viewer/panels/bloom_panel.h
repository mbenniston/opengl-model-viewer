#pragma once
#include "ipanel.h"
#include "../settings/bloom_settings.h"

class BloomPanel : public IPanel
{
public:
	BloomPanel(BloomSettings& settings);

	virtual void onDraw();

private:
	BloomSettings& m_settings;
};