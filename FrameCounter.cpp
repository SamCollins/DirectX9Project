#include "FrameCounter.h"
/*FrameCounter Class represents the frame counter that displays the current frames per second
the game*/

/*Creates a new frame counter object*/
FrameCounter::FrameCounter(LPDIRECT3DDEVICE9 pDevice)
	: fps(0)
	, startTime(0)
	, currentTime(0)
	, timeDiff(0)
	, frameRate(0)
{
	LPCTSTR typeface = "Serif";
	HRESULT test = D3DXCreateFont(pDevice, 24, 0, FW_BOLD, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);

	textFormat = DT_RIGHT | DT_TOP;
	textColor = D3DCOLOR_RGBA(255, 255, 255, 0);
}

FrameCounter::~FrameCounter()
{
}

/*Increments the frame counter*/
void FrameCounter::incFPS()
{
	fps++;
}

/*Renders the fps counter and draws it into the top left corner of the window*/
void FrameCounter::displayFPS(LPRECT pRect)
{
	//if (fps == 12) { fps = 6; } else { fps = 12; }

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	timeDiff = (currentTime / 1000) - (startTime / 1000);

	if (timeDiff >= 1000) 
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
		frameRate = fps;
		fps = 0;
	}

	string fcount = "FPS: ";
	fcount += to_string(frameRate);
	label = fcount.c_str();
	int length = fcount.length();

	int test = font->DrawText(NULL, label, -1, pRect, DT_RIGHT | DT_TOP, 0xFFFFFFFF);
	//fps = 0;
}

/*Begins the frame counter timer during initialization*/
void FrameCounter::startTimer()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
}