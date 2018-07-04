#include "Game.h"
/*This class represents the game class and contains functions for creation of the game, the game loop,
And functions needed for rendering the game*/

/*Creates a new instance of the game class

Any functions returning S_OK are for error checking purposes*/

Game::Game()
	: g_pD3D(0)
	, g_pDevice(0)
{
	//Change back to windowrect with g_hwdmain
	SetRect(&rect, 0, 0, GWND_WIDTH, GWND_HEIGHT);

	numModels = 4;
	modI = 0;
	models = new Model*[numModels];

	letItSnow = false;
}

/*This is the destructor for Game
Called during the shutdown function
Deallocates memory used in the game*/
Game::~Game()
{
	delete fc;

	delete tiger;
	delete chair;
	delete globe;
	delete drone;

	delete[] models;

	delete light;
	delete pointlight;
	delete spotlight;

	delete snow;

	delete mirror;
}

//FAILED is a macro that returns false if return value is a failure - safer than using value itself

/*Initializes the game class by creating the device that the game is being run on. 

Also initializes the frame counter, models, and lights.

g_HwndMain - is the handle to the game window

*/
int Game::Init(HWND g_hWndMain)
{
	HRESULT r = 0;//return values

	//fps
	//GetWindowRect(g_hWndMain, &rect);

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);//COM object
	if (g_pD3D == NULL) 
	{
		Utility::SetError("Could not create IDirect3D9 object");
		return E_FAIL;
	}

	//Make macros later ----------------------------------------------------------------------
	r = InitDirect3DDevice(g_hWndMain, GWND_WIDTH, GWND_HEIGHT, true, D3DFMT_X8R8G8B8, g_pD3D, &g_pDevice); //true is for windowed or not
	if (FAILED(r)) 
	{
		Utility::SetError("Initialization of the device failed");
		return E_FAIL;
	}

	// Turn on the zbuffer
	g_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	//Turn on lighting
	g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Turn on ambient lighting 
	g_pDevice->SetRenderState(D3DRS_AMBIENT, 0x888888ff);

	fc = new FrameCounter(g_pDevice);
	//fc->displayFPS(&rect);
	fc->startTimer();

	//Models
	tiger = new Model("tiger2.x");
	models[0] = tiger;
	chair = new Model("chair.x");
	models[1] = chair;
	globe = new Model("sphere.x");
	models[2] = globe;
	drone = new Model("EvilDrone.x");
	models[3] = drone;

	for (int i = 0; i < numModels; i++)
	{
		models[i]->InitGeometry(g_pDevice);
		models[i]->CreateBSphere();
	}

	//Lights
	light = new Light();
	light->InitLight(g_pDevice);
	pointlight = new PointLight();
	pointlight->InitLight(g_pDevice);
	spotlight = new SpotLight();
	spotlight->InitLight(g_pDevice);

	//Particles
	snow = new Snow(2000);
	snow->init(g_pDevice, "snowflake.dds");

	//Mirrors
	//mirror = new Mirror();
	//mirror->InitMirror(g_pDevice, models);
	//mirror->Setup(g_pDevice, models);

	return S_OK;
}

/*Loops to render the surfaces, models, and frame counter each frame. 
Also listens for:
- a quit message being sent to the window message loop
- key inputs to switch between and move the models
- turn lights on/off
*/
int Game::Loop()
{
	fc->incFPS();
	Render();
	//Quit
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	//Translations
	if (GetAsyncKeyState(0x57)) // w key - forward
	{
		models[modI]->moveForward(g_pDevice);
	}
	if (GetAsyncKeyState(0x41)) // a key - right
	{
		models[modI]->moveLeft(g_pDevice);
	}
	if (GetAsyncKeyState(0x53)) // s key - back
	{
		models[modI]->moveBack(g_pDevice);
	}
	if (GetAsyncKeyState(0x44)) // d key - left
	{
		models[modI]->moveRight(g_pDevice);
	}
	if (GetAsyncKeyState(0x45)) // e key - up
	{
		models[modI]->moveUp(g_pDevice);
	}
	if (GetAsyncKeyState(0x51)) // q key - down
	{
		models[modI]->moveDown(g_pDevice);
	}

	//Rotations
	if (GetAsyncKeyState(0x55)) // u key - rotate X pos
	{
		models[modI]->rotateXpos(g_pDevice);
	}
	if (GetAsyncKeyState(0x49)) // i key - rotate Y pos
	{
		models[modI]->rotateYpos(g_pDevice);
	}
	if (GetAsyncKeyState(0x4F)) // o key - rotate Z pos
	{
		models[modI]->rotateZpos(g_pDevice);
	}
	if (GetAsyncKeyState(0x4A)) // j key - rotate X neg
	{
		models[modI]->rotateXneg(g_pDevice);
	}
	if (GetAsyncKeyState(0x4B)) // k key - rotate Y neg
	{
		models[modI]->rotateYneg(g_pDevice);
	}
	if (GetAsyncKeyState(0x4C)) // l key - rotate Z neg
	{
		models[modI]->rotateZneg(g_pDevice);
	}

	//Switch between models
	if (GetAsyncKeyState(0x31)) // num 1 - select model 1 (tiger)
	{
		modI = 0;
	}
	if (GetAsyncKeyState(0x32)) // num 2 - select model 2 (chair)
	{
		modI = 1;
	}
	if (GetAsyncKeyState(0x33)) // num 3 - select model 3 (globe)
	{
		modI = 2;
	}
	if (GetAsyncKeyState(0x34)) // num 4 - select model 4 (pawn)
	{
		modI = 3;
	}

	//Lights
	if (GetAsyncKeyState(VK_F1)) // f1 - ambient light (red)
	{
		g_pDevice->SetRenderState(D3DRS_AMBIENT, 0x888888ff);
	}
	if (GetAsyncKeyState(VK_F2)) // f2 - ambient light (red)
	{
		g_pDevice->SetRenderState(D3DRS_AMBIENT, 0);
	}
	if (GetAsyncKeyState(VK_F3)) // f3 - directional light on (red)
	{
		light->lightOn(g_pDevice);
	}
	if (GetAsyncKeyState(VK_F4)) // f4 - directional light off (red)
	{
		light->lightOff(g_pDevice);
	}
	if (GetAsyncKeyState(VK_F5)) // f5 - point light on (blue)
	{
		pointlight->lightOn(g_pDevice);
	}
	if (GetAsyncKeyState(VK_F6)) // f6 - point light off (blue)
	{
		pointlight->lightOff(g_pDevice);
	}
	if (GetAsyncKeyState(VK_F7)) // f7 - spot light on (green)
	{
		spotlight->lightOn(g_pDevice);
	}
	if (GetAsyncKeyState(VK_F8)) // f8 - spot light off (green)
	{
		spotlight->lightOff(g_pDevice);
	}

	//Snow
	if (GetAsyncKeyState(VK_F9)) // f9 - Turn on snowing
	{
		letItSnow = true;
	}
	if (GetAsyncKeyState(VK_F10)) // f10 - Turn off snowing
	{
		letItSnow = false;
	}

	return S_OK;
}

/*Releases the devices and memory that are being use to create/run the game
*/
int Game::Shutdown()
{
	//release resources. First display adapter because COM object created it, then COM object
	if (g_pDevice)
		g_pDevice->Release();

	if (g_pD3D)
		g_pD3D->Release();

	return S_OK;
}

/*Renders each frame by loading the neccessary surfaces for the background. 
Also renders any models and lights that exist and displays the current FPS
*/
int Game::Render()
{
	HRESULT r;
	D3DLOCKED_RECT LockedRect;			//locked area of display memory(buffer really) we are drawing to
	LPDIRECT3DSURFACE9 pBackSurf = 0;
	LPDIRECT3DSURFACE9 pSurface = 0;

	if (!g_pDevice) 
	{
		Utility::SetError("Cannot render because there is no device");
		return E_FAIL;
	}

	//Update Snow
	if (letItSnow)
		snow->update(0.1f);

	//clear the display arera with colour black, ignore stencil buffer
	//Need D3D ClearStencil
	g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 25), 1.0f, 0);

	#pragma region Surface
	//g_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME); - To show wireframe

	//get pointer to backbuffer------------------------------------------------------
	//r = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurf);
	//if (FAILED(r)) 
	//{
	//	Utility::SetError("Couldn't get backbuffer");
	//}

	////Loads the bitmap onto the surface initially
	//r = LoadBitmapToSurface("baboon.bmp", &pSurface, g_pDevice);
	//if (FAILED(r)) {
	//	Utility::SetError("could not load bitmap surface");
	//}

	//r = D3DXLoadSurfaceFromSurface(pBackSurf, NULL, NULL, pSurface, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
	//if (FAILED(r))
	//	Utility::SetError("did not copy surface");-----------------------------------

	#pragma endregion

	//FPS counter
	g_pDevice->BeginScene();

	//Render all models
	for (int i = 0; i < numModels; i++)
	{
		models[i]->SetupMatrices(g_pDevice);
		models[i]->RenderModel(g_pDevice);
	}

	//Render Mirrors
	//mirror->DrawMirror();
	//mirror->Render();

	//mirror->TestScene();
	//mirror->TestMirror();

	if (letItSnow)
		snow->render();
	
	g_pDevice->EndScene();

	fc->displayFPS(&rect);


	////get a lock on the surface-------------------------------
	//r = pBackSurf->LockRect(&LockedRect, NULL, 0);
	//if (FAILED(r)) 
	//{
	//	Utility::SetError("Could not lock the back buffer");
	//}

	////DWORD* pData = (DWORD*)(LockedRect.pBits);
	////DRAW CODE GOES HERE - use pData
	////Draw(LockedRect.Pitch, pData);

	//pBackSurf->UnlockRect();
	////pData = 0;

	//pBackSurf->Release();//release lock

	//pBackSurf = 0;------------------------------------

	g_pDevice->Present(NULL, NULL, NULL, NULL);//swap over buffer to primary surface
	return S_OK;
}



/*Draws onto the surface by accessing the individual pixels of the bitmap

Pitch - is the row of pixels being drawn to
pData - is the buffer holding the data being drawn to
*/
void Game::Draw(int Pitch, DWORD* pData) 
{
	DWORD Offset = 100 * Pitch / 4 + 200;
	//pData[Offset] = D3DCOLOR_XRGB(255, 0, 0);

	for (int i = 0; i < 100; i++)
	{
		pData[Offset] = D3DCOLOR_XRGB(0, 255, 0);
		Offset++;
	}

	//SimpleBitmapDraw();
}

/*Initializes the device being used to hold the surface so that models can be loaded onto it

hWndTarget- the handle to the device being created
width - the width of the window
height- the height of the window
bWindowed - toggles if the game is fullscreen
FullScreenFormat - formats the game to be fullscreened
pD3d - the device
ppDevice - a pointer to the device being initialized
*/
int Game::InitDirect3DDevice(HWND hWndTarget, int Width, int Height, bool bWindowed, D3DFORMAT FullScreenFormat,
	LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9* ppDevice)
{
	D3DPRESENT_PARAMETERS d3dpp;//rendering info
	D3DDISPLAYMODE d3ddm;//current display mode info
	HRESULT r = 0;

	if (*ppDevice)
		(*ppDevice)->Release();

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	r = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(r)) {
		Utility::SetError("Could not get display adapter information");
		return E_FAIL;
	}

	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = bWindowed ? d3ddm.Format : FullScreenFormat;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_COPY;
	d3dpp.hDeviceWindow = hWndTarget;
	d3dpp.Windowed = bWindowed;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.FullScreen_RefreshRateInHz = 0;//default refresh rate
	d3dpp.PresentationInterval = bWindowed ? 0 : D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	r = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndTarget, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, ppDevice);
	if (FAILED(r)) {
		Utility::SetError("Could not create the render device");
		return E_FAIL;
	}

	//	g_DeviceHeight = Height;
	//	g_DeviceWidth = Width;

	//	g_SavedPresParams = d3dpp;
	return S_OK;
}

/*Loads a bitmap onto a surface by retrieving a bitmap from the files and strtching it to fit the surface

pathname - the path to the bitmap
ppSurface - a pointer to the surface the bitmap is being drawn to
pDevice - a pointer to the device the surface belongs to
*/
int Game::LoadBitmapToSurface(string pathName, LPDIRECT3DSURFACE9* ppSurface, LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT r;
	HBITMAP hBitmap;
	BITMAP Bitmap;

	//hBitmap = (HBITMAP)LoadImage(NULL, pathName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	//if (hBitmap == NULL) {
	//	Utility::SetError("Unable to load bitmap");
	//	return E_FAIL;
	//}

	//GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	//DeleteObject(hBitmap);//we only needed it for the header info to create a D3D surface

						  //create surface for bitmap
	r = pDevice->CreateOffscreenPlainSurface(GWND_WIDTH, GWND_HEIGHT, D3DFMT_X8R8G8B8, D3DPOOL_SCRATCH, ppSurface, NULL);

	if (FAILED(r)) {
		Utility::SetError("Unable to create surface for bitmap load");
		return E_FAIL;
	}
	//load bitmap onto surface
	r = D3DXLoadSurfaceFromFile(*ppSurface, NULL, NULL, pathName.c_str(), NULL, D3DX_DEFAULT, 0, NULL);
	if (FAILED(r)) {
		Utility::SetError("Unable to load file to surface");
		return E_FAIL;
	}

	return S_OK;
}

#pragma region Rays

void Game::GetRay(int x, int y)
{
	// compute the ray in view space given the clicked screen point
	Ray ray = CalcPickingRay(x, y);

	// transform the ray to world space
	D3DXMATRIX view;
	g_pDevice->GetTransform(D3DTS_VIEW, &view);

	D3DXMATRIX viewInverse;
	D3DXMatrixInverse(&viewInverse, 0, &view);

	TransformRay(&ray, &viewInverse);

	// test for a hit
	for (int i = 0; i < numModels; i++)
	{
		//Set current model to be whatever was clicked on
		if (RaySphereIntTest(&ray, models[i]->GetBSphere()))
		{
			modI = i;
		}
			//::MessageBox(0, "Hit!", "HIT", 0);
	}
}

Ray Game::CalcPickingRay(int x, int y)
{
	float px = 0.0f;
	float py = 0.0f;

	D3DVIEWPORT9 vp;
	g_pDevice->GetViewport(&vp);

	D3DXMATRIX proj;
	g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);

	px = (((2.0f*x) / vp.Width) - 1.0f) / proj(0, 0);
	py = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);

	Ray ray;
	ray._origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ray._direction = D3DXVECTOR3(px, py, 1.0f);

	return ray;
}

void Game::TransformRay(Ray* ray, D3DXMATRIX* T)
{
	// transform the ray's origin, w = 1.
	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		T);

	// transform the ray's direction, w = 0.
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		T);

	// normalize the direction
	D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}

bool Game::RaySphereIntTest(Ray* ray, BoundingSphere* sphere)
{
	D3DXVECTOR3 v = ray->_origin - sphere->_center;

	float b = 2.0f * D3DXVec3Dot(&ray->_direction, &v);
	float c = D3DXVec3Dot(&v, &v) - (sphere->_radius * sphere->_radius);

	// find the discriminant
	float discriminant = (b * b) - (4.0f * c);

	// test for imaginary number
	if (discriminant < 0.0f)
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// if a solution is >= 0, then we intersected the sphere
	if (s0 >= 0.0f || s1 >= 0.0f)
		return true;

	return false;
}

#pragma endregion
