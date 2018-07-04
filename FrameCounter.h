#pragma once

#include "basics.h"
#include "D3dx9core.h"

class FrameCounter{
public:
	FrameCounter(LPDIRECT3DDEVICE9 pDevice);
	~FrameCounter();
	void incFPS();
	void displayFPS(LPRECT pRect);

	void startTimer();

private:
	LPD3DXFONT font;

	int fps;
	LPCTSTR label;
	DWORD textFormat;
	D3DCOLOR textColor;

	int64_t startTime;
	int64_t currentTime;
	int timeDiff;
	int frameRate;
};