#include "Model.h"
/*Represents a model loaded in from a .x file, has functions for initializing the
shapes/textures needed to render the model

Also contains functions that handle transformations of the model.*/


/*Creates a model after being given a string that is the name of the .x file for the model

Also initializes the transformation matrix that determines the rendering position of the model
*/
Model::Model(string xFile)
	: g_pMesh(0)
	, g_pMeshMaterials(0)
	, g_pMeshTextures(0)
	, g_dwNumMaterials(0L)
	, mxFile(xFile)
{
	D3DXMatrixIdentity(&master);
}

/*Deallocates the resources that the model uses
*/
Model::~Model()
{
	Cleanup();
}

/*Loads in the mesh and textures from the .x file

g_pDevice is the direct3d device used for rendering
*/
HRESULT Model::InitGeometry(LPDIRECT3DDEVICE9 g_pDevice)
{
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	if (FAILED(D3DXLoadMeshFromX(mxFile.c_str(), D3DXMESH_SYSTEMMEM,
		g_pDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
		&g_pMesh)))
	{
		// If model is not in current folder, try parent folder
		if (FAILED(D3DXLoadMeshFromX("..\\Tiger.x", D3DXMESH_SYSTEMMEM,
			g_pDevice, NULL,
			&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
			&g_pMesh)))
		{
			MessageBox(NULL, "Could not find tiger.x", "Meshes.exe", MB_OK);
			return E_FAIL;
		}
	}

	// We need to extract the material properties and texture names from the 
	// pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

	for (DWORD i = 0; i<g_dwNumMaterials; i++)
	{
		// Copy the material
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			// Create the texture
			if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
				d3dxMaterials[i].pTextureFilename,
				&g_pMeshTextures[i])))
			{
				// If texture is not in current folder, try parent folder
				const TCHAR* strPrefix = TEXT("..\\");
				const int lenPrefix = lstrlen(strPrefix);
				TCHAR strTexture[MAX_PATH];
				lstrcpyn(strTexture, strPrefix, MAX_PATH);
				lstrcpyn(strTexture + lenPrefix, d3dxMaterials[i].pTextureFilename, MAX_PATH - lenPrefix);
				// If texture is not in current folder, try parent folder
				if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
					strTexture,
					&g_pMeshTextures[i])))
				{
					MessageBox(NULL, "Could not find texture map", "Meshes.exe", MB_OK);
				}
			}
		}
	}

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	return S_OK;
}

/*Sets the transormation matrix for the model

g_pDevice is the direct3d device used for rendering
*/
void Model::RenderModel(LPDIRECT3DDEVICE9 g_pDevice)
{
	//Make a master matrix in this function as a member
	//make functions that alter the transformation matrix 
	//Use the directx matrix generation/multiplication functions
	g_pDevice->SetTransform(D3DTS_WORLD, &master);

	//Translate BSphere to same spot as model
	//Gets messed up by rotation, fix later
	BSphere._center = D3DXVECTOR3(master._41, master._42, master._43);

	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		// Set the material and texture for this subset
		g_pDevice->SetMaterial(&g_pMeshMaterials[i]);
		g_pDevice->SetTexture(0, g_pMeshTextures[i]);

		// Draw the mesh subset
		g_pMesh->DrawSubset(i);
	}
}

/*Creates the initial matrix for the rendering position of the model, the camera,
and the field of view

g_pDevice is the direct3d device used for rendering
*/
void Model::SetupMatrices(LPDIRECT3DDEVICE9 g_pDevice)
{
	// For our world matrix, we will just leave it as the identity
	D3DXMATRIXA16 matWorld;
	//D3DXMatrixRotationY(&matWorld, 0 / 1000.0f); //timeGetTime()
	g_pDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the 
	// origin, and define "up" to be in the y-direction.
	//MY VIEW
	D3DXVECTOR3 vEyePt(0.0f, 6.0f, 10.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pDevice->SetTransform(D3DTS_VIEW, &matView);
	
	//EXAMPLE VIEW
	/*D3DXVECTOR3    pos(-10.0f, 3.0f, -15.0f);
	D3DXVECTOR3 target(0.0, 0.0f, 0.0f);
	D3DXVECTOR3     up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &pos, &target, &up);
	g_pDevice->SetTransform(D3DTS_VIEW, &V);*/

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void Model::CreateBSphere()
{
	BYTE* v;
	g_pMesh->LockVertexBuffer(0, (void**)&v);

	D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		g_pMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(g_pMesh->GetFVF()),
		&BSphere._center,
		&BSphere._radius);

	g_pMesh->UnlockVertexBuffer();
}

BoundingSphere* Model::GetBSphere()
{
	return &BSphere;
}

/*Deallocates the resources used by the model*/
void Model::Cleanup()
{
	if (g_pMeshMaterials != NULL)
		delete[] g_pMeshMaterials;

	if (g_pMeshTextures)
	{
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			if (g_pMeshTextures[i])
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	if (g_pMesh != NULL)
		g_pMesh->Release();
}

BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

#pragma region Movement functions

/*Translates the model to the right along x axis
g_pDevice - the direct3d device used for rendering
*/
void Model::moveRight(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 trans;
	D3DXMatrixTranslation(&trans, -0.2f, 0, 0);
	D3DXMatrixMultiply(&master, &master, &trans);
}
/*Translates the model to the left along x axis
g_pDevice - the direct3d device used for rendering
*/
void Model::moveLeft(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 trans;
	D3DXMatrixTranslation(&trans, 0.2f, 0, 0);
	D3DXMatrixMultiply(&master, &master, &trans);
}
/*Translates the model forward along z axis
g_pDevice - the direct3d device used for rendering
*/
void Model::moveForward(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 trans;
	D3DXMatrixTranslation(&trans, 0, 0, -0.2f);
	D3DXMatrixMultiply(&master, &master, &trans);
}
/*Translates the model backward along z axis
g_pDevice - the direct3d device used for rendering
*/
void Model::moveBack(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 trans;
	D3DXMatrixTranslation(&trans, 0, 0, 0.2f);
	D3DXMatrixMultiply(&master, &master, &trans);
}
/*Translates the model up along y axis
g_pDevice - the direct3d device used for rendering
*/
void Model::moveUp(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 trans;
	D3DXMatrixTranslation(&trans, 0, 0.2f, 0);
	D3DXMatrixMultiply(&master, &master, &trans);
}
/*Translates the model down along y axis
g_pDevice - the direct3d device used for rendering
*/
void Model::moveDown(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 trans;
	D3DXMatrixTranslation(&trans, 0, -0.2f, 0);
	D3DXMatrixMultiply(&master, &master, &trans);
}

/*Rotates the model CW around the x axis
g_pDevice - the direct3d device used for rendering
*/
void Model::rotateXpos(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 rotate;
	D3DXMatrixRotationX(&rotate, 0.1f);
	D3DXMatrixMultiply(&master, &master, &rotate);
}
/*Rotates the model CW around the y axis
g_pDevice - the direct3d device used for rendering
*/
void Model::rotateYpos(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 rotate;
	D3DXMatrixRotationY(&rotate, 0.1f);
	D3DXMatrixMultiply(&master, &master, &rotate);
}
/*Rotates the model CW around the z axis
g_pDevice - the direct3d device used for rendering
*/
void Model::rotateZpos(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 rotate;
	D3DXMatrixRotationZ(&rotate, 0.1f);
	D3DXMatrixMultiply(&master, &master, &rotate);
}
/*Rotates the model CCW around the x axis
g_pDevice - the direct3d device used for rendering
*/
void Model::rotateXneg(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 rotate;
	D3DXMatrixRotationX(&rotate, -0.1f);
	D3DXMatrixMultiply(&master, &master, &rotate);
}
/*Rotates the model CCW around the y axis
g_pDevice - the direct3d device used for rendering
*/
void Model::rotateYneg(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 rotate;
	D3DXMatrixRotationY(&rotate, -0.1f);
	D3DXMatrixMultiply(&master, &master, &rotate);
}
/*Rotates the model CCW around the z axis
g_pDevice - the direct3d device used for rendering
*/
void Model::rotateZneg(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DXMATRIXA16 rotate;
	D3DXMatrixRotationZ(&rotate, -0.1f);
	D3DXMatrixMultiply(&master, &master, &rotate);
}

#pragma endregion
