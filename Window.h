#pragma once

#include "basics.h"
#include "Game.h"

class Window
{
public:
	Window(HINSTANCE hInstance, int iCmdShow);
	~Window();

	int StartGame();
	int MsgLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
	Game* game;
	HWND g_hWndMain;		//handle to main window
};