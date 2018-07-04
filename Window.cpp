#include "Window.h"

/*Creates and registers a window using win32 and marks it to be drawn*/
Window::Window(HINSTANCE hInstance, int iCmdShow)
{
	HWND hWnd;
	//MSG msg;
	WNDCLASSEX wc;

	static char strAppName[] = "First Windows App, Zen Style";

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_HAND);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = strAppName;

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		strAppName,
		strAppName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		512, 512,
		NULL,
		NULL,
		hInstance,
		NULL);

	g_hWndMain = hWnd;//set our global window handle

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);
}

Window::~Window()
{
	delete game;
}

/*Creates a new game object

Returns an error code indicating if the functions were successful
*/
int Window::StartGame()
{
	game = new Game();
	
	if (FAILED(game->Init(g_hWndMain))) 			//initialize Game
	{
		Utility::SetError("Initialization Failed");
		game->Shutdown();
		return E_FAIL;
	}

	return S_OK;
}

/*Begins the window message loop
*/
int Window::MsgLoop()
{
	MSG msg;
	while (true) 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			if (msg.message == WM_QUIT)
				break;
			if (msg.message == WM_LBUTTONDOWN)
			{
				game->GetRay(LOWORD(msg.lParam), HIWORD(msg.lParam));
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			game->Loop();
		}
	}
	game->Shutdown();
	return msg.wParam;
}

/*The wndProc that messages passed to the reistered window class are passed
through before being sent to be handled by the default procedures.
*/
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage) {
		case WM_CREATE:
		{
			return 0;
		}
		case WM_PAINT:
		{
			ValidateRect(hWnd, NULL);//basically saying - yeah we took care of any paint msg without any overhead
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return DefWindowProc(hWnd, uMessage, wParam, lParam);
		}
	}
}

