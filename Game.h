#pragma once

#include "basics.h"
#include "Utility.h"
#include "FrameCounter.h"
#include "Model.h"
#include "Light.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Snow.h"
#include "Mirror.h"

#define GWND_WIDTH 500
#define GWND_HEIGHT 500
#define WINDOWED false

struct Ray
{
	D3DXVECTOR3 _origin;
	D3DXVECTOR3 _direction;
};

class Game 
{
public:
	Game();
	~Game();

	int Init(HWND g_hWndMain);
	int Loop();
	int Shutdown();

	int Render();
	void Draw(int Pitch, DWORD* pData);

	int LoadBitmapToSurface(string pathName, LPDIRECT3DSURFACE9* ppSurface, LPDIRECT3DDEVICE9 pDevice);

	void GetRay(int x, int y);


private:
	LPDIRECT3D9 g_pD3D;				//COM object
	LPDIRECT3DDEVICE9 g_pDevice;	//graphics device

	int InitDirect3DDevice(HWND hWndTarget, int Width, int Height, bool bWindowed, D3DFORMAT FullScreenFormat,
		LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9* ppDevice);

	Ray CalcPickingRay(int x, int y);
	void TransformRay(Ray* ray, D3DXMATRIX* T);
	bool RaySphereIntTest(Ray* ray, BoundingSphere* sphere);

	//fps
	FrameCounter* fc;
	//HWND hWnd;
	RECT rect;

	//Models
	Model* tiger;
	Model* chair;
	Model* globe;
	Model* drone;

	Model** models;
	int modI;
	int numModels;

	//Lights
	Light* light;
	PointLight* pointlight;
	SpotLight* spotlight;

	//Particles
	Snow* snow;
	bool letItSnow;

	//Mirrors
	Mirror* mirror;
};

