#include "Light.h"
/*Creates a directional light in the directx environment*/

/*Creates a new Lighting object*/
Light::Light()
	: bLight(false)
{
}

Light::~Light()
{

}

/*Initializes the light by setting the type of light it is and the color of the light

g_pDevice - the direct3d device used for rendering
*/
void Light::InitLight(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));

	//Set up light
	Light.Type = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse = D3DXCOLOR(1.0, 0, 0, 1.0);
	//Light.Ambient = color;
	//Light.Range = 100.0f;
	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(
		0.0f,
		-1.0f,
		0.0f
	);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);
	//attach light structure to a Direct3D Lighting index

	g_pDevice->SetLight(0, &Light);

	//enable light
	//g_pDevice->LightEnable(0, TRUE);
}

/*Turns the light on by calling the LightEnable function

g_pDevice - the direct3d device used for rendering
*/
void Light::lightOn(LPDIRECT3DDEVICE9 g_pDevice)
{
	g_pDevice->LightEnable(0, true);
}

/*Turns the light off by calling the LightEnable function

g_pDevice - the direct3d device used for rendering
*/
void Light::lightOff(LPDIRECT3DDEVICE9 g_pDevice)
{
	g_pDevice->LightEnable(0, false);
}
