#pragma once

#include "basics.h"

struct BoundingSphere
{
	BoundingSphere();

	D3DXVECTOR3 _center;
	float       _radius;
};

class Model {

public:
	Model(string xFile);
	Model();
	~Model();

	HRESULT InitGeometry(LPDIRECT3DDEVICE9 g_pDevice);
	void SetupMatrices(LPDIRECT3DDEVICE9 g_pDevice);
	void RenderModel(LPDIRECT3DDEVICE9 g_pDevice);
	void CreateBSphere();
	BoundingSphere* GetBSphere();

	void moveRight(LPDIRECT3DDEVICE9 g_pDevice);
	void moveLeft(LPDIRECT3DDEVICE9 g_pDevice);
	void moveForward(LPDIRECT3DDEVICE9 g_pDevice);
	void moveBack(LPDIRECT3DDEVICE9 g_pDevice);
	void moveUp(LPDIRECT3DDEVICE9 g_pDevice);
	void moveDown(LPDIRECT3DDEVICE9 g_pDevice);

	void rotateXpos(LPDIRECT3DDEVICE9 g_pDevice);
	void rotateYpos(LPDIRECT3DDEVICE9 g_pDevice);
	void rotateZpos(LPDIRECT3DDEVICE9 g_pDevice);
	void rotateXneg(LPDIRECT3DDEVICE9 g_pDevice);
	void rotateYneg(LPDIRECT3DDEVICE9 g_pDevice);
	void rotateZneg(LPDIRECT3DDEVICE9 g_pDevice);

	void Cleanup();

	//Members
	LPD3DXMESH              g_pMesh; // Our mesh object in sysmem
	D3DMATERIAL9*           g_pMeshMaterials; // Materials for our mesh
	LPDIRECT3DTEXTURE9*     g_pMeshTextures; // Textures for our mesh
	DWORD                   g_dwNumMaterials;   // Number of mesh materials

	string mxFile;

	D3DXMATRIXA16 master;

	BoundingSphere BSphere;

private:
};

