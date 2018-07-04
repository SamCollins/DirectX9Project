//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: stencilmirror.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates mirrors with stencils.  Use the arrow keys
//       and the 'A' and 'S' key to navigate the scene and translate the teapot.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

#define len 2.5f

//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 640;
const int Height = 480;

IDirect3DVertexBuffer9* VB = 0;

IDirect3DTexture9* FloorTex = 0;
IDirect3DTexture9* WallTex = 0;
IDirect3DTexture9* MirrorTex = 0;

D3DMATERIAL9 FloorMtrl = d3d::WHITE_MTRL;
D3DMATERIAL9 WallMtrl = d3d::WHITE_MTRL;
D3DMATERIAL9 MirrorMtrl = d3d::WHITE_MTRL;

ID3DXMesh* Teapot = 0;
D3DXVECTOR3 TeapotPosition(0.0f, 0.0f, 7.5f);
D3DMATERIAL9 TeapotMtrl = d3d::YELLOW_MTRL;

void RenderScene();
void RenderMirror(int i, float xp, float yp, float zp, float mx, float my, float mz);


int Sign(float x)
{
	return (x > 0) - (x < 0);
}

//
// Classes and Structures
//
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
//const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

//
// Framework Functions
//
bool Setup()
{

	//
	// Make walls have low specular reflectance - 20%.
	//

	WallMtrl.Specular = d3d::WHITE * 0.2f;

	//
	// Create the teapot.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/
	//
	Device->CreateVertexBuffer(
		36 * sizeof(Vertex),
		0, // usage
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	Vertex* v = 0;
	VB->Lock(0, 0, (void**)&v, 0);

	// mirror

	//Front face
	v[0] = Vertex(len, -len, len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(len, len, len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(-len, len, len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(len, -len, len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(-len, len, len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(-len, -len, len, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	//right face
	v[6] = Vertex(-len, -len, len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-len, len, len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-len, len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-len, -len, len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-len, len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-len, -len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	//top face
	v[12] = Vertex(len, len, len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(len, len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(-len, len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(len, len, len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(-len, len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(-len, len, len, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	//back face
	v[18] = Vertex(-len, -len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-len, len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(len, len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-len, -len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(len, len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(len, -len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	//left face
	v[24] = Vertex(len, -len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[25] = Vertex(len, len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[26] = Vertex(len, len, len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[27] = Vertex(len, -len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[28] = Vertex(len, len, len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[29] = Vertex(len, -len, len, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	//bottom face
	v[30] = Vertex(len, -len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[31] = Vertex(len, -len, len, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[32] = Vertex(-len, -len, len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[33] = Vertex(len, -len, -len, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[34] = Vertex(-len, -len, len, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[35] = Vertex(-len, -len, -len, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	//
	// Load Textures, set filters.
	//

	D3DXCreateTextureFromFile(Device, "checker.jpg", &FloorTex);
	D3DXCreateTextureFromFile(Device, "brick0.jpg", &WallTex);
	D3DXCreateTextureFromFile(Device, "ice.bmp", &MirrorTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//
	// Lights.
	//

	D3DXVECTOR3 lightDir(0.707f, -0.707f, 0.707f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set Camera.
	//

	D3DXVECTOR3    pos(0.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
	D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI / 4.0f, // 45 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DTexture9*>(FloorTex);
	d3d::Release<IDirect3DTexture9*>(WallTex);
	d3d::Release<IDirect3DTexture9*>(MirrorTex);
	d3d::Release<ID3DXMesh*>(Teapot);
}

bool Display(float timeDelta)
{
	if (Device)
	{
		//
		// Update the scene:
		//

		static float radius = 20.0f;
		static float angle = (1.0f * D3DX_PI) / 2.0f;
		static float pitch = (1.0f * D3DX_PI) / 2.0f;

		//Camera Movement
		if (::GetAsyncKeyState('W') & 0x8000f)
			pitch += 0.5f * timeDelta;

		if (::GetAsyncKeyState('A') & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (::GetAsyncKeyState('S') & 0x8000f)
			pitch -= 0.5f * timeDelta;

		if (::GetAsyncKeyState('D') & 0x8000f)
			angle += 0.5f * timeDelta;

		if (::GetAsyncKeyState('E') & 0x8000f)
			radius -= 2.0f * timeDelta;

		if (::GetAsyncKeyState('Q') & 0x8000f)
			radius += 2.0f * timeDelta;

		//Teapot movement
		if (::GetAsyncKeyState('J') & 0x8000f)
			TeapotPosition.x += 3.0f * timeDelta;

		if (::GetAsyncKeyState('L') & 0x8000f)
			TeapotPosition.x -= 3.0f * timeDelta;

		if (::GetAsyncKeyState('K') & 0x8000f)
			TeapotPosition.z += 3.0f * timeDelta;

		if (::GetAsyncKeyState('I') & 0x8000f)
			TeapotPosition.z -= 3.0f * timeDelta;

		if (::GetAsyncKeyState('O') & 0x8000f)
			TeapotPosition.y += 3.0f * timeDelta;

		if (::GetAsyncKeyState('U') & 0x8000f)
			TeapotPosition.y -= 3.0f * timeDelta;

		D3DXVECTOR3 position(cosf(angle) * radius, cosf(pitch) * radius, sinf(angle) * radius);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x000000, 1.0f, 0L); 
		Device->Clear(0, 0, D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0L);

		Device->BeginScene();

		RenderScene();

		RenderMirror(1, 0, 0, 1, 0, 0, len);
		RenderMirror(2, -1, 0, 0, -len, 0, 0);
		RenderMirror(3, 0, 1, 0, 0, len, 0);

		RenderMirror(4, 0, 0, -1, 0, 0, -len);
		RenderMirror(5, 1, 0, 0, len, 0, 0);
		RenderMirror(6, 0, -1, 0, 0, -len, 0);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

void RenderScene()
{
	// draw teapot
	Device->SetMaterial(&TeapotMtrl);
	Device->SetTexture(0, 0);
	D3DXMATRIX W;
	D3DXMatrixTranslation(&W,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);

	Device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);

	//Teapot 2
	D3DXMatrixIdentity(&W);
	D3DXMatrixTranslation(&W,
		TeapotPosition.x,
		TeapotPosition.y,
		-TeapotPosition.z);

	Device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);

	//done teapots
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);

	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);

	// draw the mirror
	Device->SetMaterial(&MirrorMtrl);
	Device->SetTexture(0, MirrorTex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 2);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 12, 2);

	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 24, 2);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 30, 2);

}

void RenderMirror(int i, float xp, float yp, float zp, float mx, float my, float mz)
{
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//

	Device->SetRenderState(D3DRS_STENCILENABLE, true);
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	Device->SetRenderState(D3DRS_STENCILREF, i);
	Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
	Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// draw the mirror to the stencil buffer
	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);
	Device->SetMaterial(&MirrorMtrl);
	Device->SetTexture(0, MirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);

	Device->DrawPrimitive(D3DPT_TRIANGLELIST, (i - 1) * 6, 2);

	// re-enable depth writes
	Device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// position reflection
	D3DXMATRIX W, T, R, MT;
	D3DXPLANE plane(xp, yp, zp, 0.0f); // xy plane
	D3DXMatrixReflect(&R, &plane);

	D3DXMatrixTranslation(&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);

	D3DXMatrixTranslation(&MT,
		(TeapotPosition.x - mx) * xp * 2 * Sign(xp),
		(TeapotPosition.y - my) * yp * 2 * Sign(yp),
		(TeapotPosition.z - mz) * zp * 2 * Sign(zp));

	W = MT * R * T;

	// clear depth buffer and blend the reflected teapot with the mirror
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// Finally, draw the reflected teapot
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&TeapotMtrl);
	Device->SetTexture(0, 0);

	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	if (Sign(TeapotPosition.x - mx) * xp * xp == Sign(xp) && 
		Sign(TeapotPosition.y - my) * yp * yp == Sign(yp) && 
		Sign(TeapotPosition.z - mz) * zp * zp == Sign(zp))
	{
		Teapot->DrawSubset(0);
	}

	D3DXMatrixTranslation(&T,
		TeapotPosition.x,
		TeapotPosition.y,
		-TeapotPosition.z);

	D3DXMatrixTranslation(&MT,
		(TeapotPosition.x - mx) * xp * 2 * Sign(xp),
		(TeapotPosition.y - my) * yp * 2 * Sign(yp),
		(-TeapotPosition.z - mz) * zp * 2 * Sign(zp));

	W = MT * R * T;
	Device->SetTransform(D3DTS_WORLD, &W);

	if (Sign(TeapotPosition.x - mx) * xp * xp == Sign(xp) &&
		Sign(TeapotPosition.y - my) * yp * yp == Sign(yp) &&
		Sign(-TeapotPosition.z - mz) * zp * zp == Sign(zp))
	{
		Teapot->DrawSubset(0);
	}

	// Restore render states.
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	if (!d3d::InitD3D(hinstance, Width, Height, true, D3DDEVTYPE_HAL, &Device)) {
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
	if (!Setup()) {
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}
	d3d::EnterMsgLoop(Display);
	Cleanup();
	Device->Release();
	return 0;
}