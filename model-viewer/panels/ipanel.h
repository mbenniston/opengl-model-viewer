#pragma once

class IPanel
{
public:
	virtual void onDraw() = 0;
	virtual ~IPanel() = default;
};