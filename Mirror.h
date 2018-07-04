#pragma once

#include "basics.h"
#include "Model.h"

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;

	static const DWORD FVF;
};

const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));


class Mirror
{
public:
	Mirror();
	~Mirror();
	void InitMirror(LPDIRECT3DDEVICE9 g_pDevice, Model** models);
	void Render();
	void DrawMirror();

	//TEST STUFF
	bool Setup(LPDIRECT3DDEVICE9 g_pDevice, Model** RefModels);
	void TestScene();
	void TestMirror();
	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);


	IDirect3DDevice9* Device = 0;

	const int Width = 640;
	const int Height = 480;

	IDirect3DVertexBuffer9* VB = 0;

	IDirect3DTexture9* FloorTex = 0;
	IDirect3DTexture9* WallTex = 0;
	IDirect3DTexture9* MirrorTex = 0;

	D3DMATERIAL9 FloorMtrl;
	D3DMATERIAL9 WallMtrl;
	D3DMATERIAL9 MirrorMtrl;

	ID3DXMesh* Teapot = 0;
	D3DXVECTOR3 TeapotPosition = D3DXVECTOR3(0.0f, 3.0f, -7.5f);
	D3DMATERIAL9 TeapotMtrl;

	//Members
	//LPDIRECT3DDEVICE9 Device;
	//IDirect3DVertexBuffer9* VB = 0;
	//IDirect3DTexture9* MirrorTex = 0;
	//D3DMATERIAL9 MirrorMtrl;

	Model** models;

};
