#include "SpotLight.h"
/*Creates a spotlight light in the directx environment*/

/*Creates a new Lighting object*/
SpotLight::SpotLight()
	: bLight(false)
{
}

SpotLight::~SpotLight()
{

}

/*Initializes the light by setting the type of light it is and the color of the light

g_pDevice - the direct3d device used for rendering
*/
void SpotLight::InitLight(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));

	//Set up light
	Light.Type = D3DLIGHT_SPOT;
	Light.Diffuse = D3DXCOLOR (0, 1.0, 0, 1.0);
	//Light.Ambient = color;
	Light.Range = 10.0f;
	Light.Position = D3DXVECTOR3(
		5.0f,
		5.0f,
		5.0f
	);
	Light.Phi = D3DXToRadian(40.0f);
	Light.Theta = D3DXToRadian(20.0f);
	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(
		-5.0f,
		-5.0f,
		0.0f
	);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);
	//attach light structure to a Direct3D Lighting index

	g_pDevice->SetLight(2, &Light);

	//enable light
	//g_pDevice->LightEnable(0, TRUE);
}

/*Turns the light on by calling the LightEnable function

g_pDevice - the direct3d device used for rendering
*/
void SpotLight::lightOn(LPDIRECT3DDEVICE9 g_pDevice)
{
	g_pDevice->LightEnable(2, true);
}

/*Turns the light off by calling the LightEnable function

g_pDevice - the direct3d device used for rendering
*/
void SpotLight::lightOff(LPDIRECT3DDEVICE9 g_pDevice)
{
	g_pDevice->LightEnable(2, false);
}
