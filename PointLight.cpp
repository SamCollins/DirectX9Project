#include "PointLight.h"
/*Creates a point light in the directx environment*/

/*Creates a new Lighting object*/
PointLight::PointLight()
	: bLight(false)
{
}

PointLight::~PointLight()
{

}

/*Initializes the light by setting the type of light it is and the color of the light

g_pDevice - the direct3d device used for rendering
*/
void PointLight::InitLight(LPDIRECT3DDEVICE9 g_pDevice)
{
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));

	//Set up light
	Light.Type = D3DLIGHT_POINT;
	Light.Diffuse = D3DXCOLOR(0, 0, 1.0, 1.0);
	//Light.Ambient = color;
	Light.Range = 15.0f;
	Light.Position = D3DXVECTOR3(
		-2.0f,
		2.0f,
		2.0f
	);
	//attach light structure to a Direct3D Lighting index

	g_pDevice->SetLight(1, &Light);

	//enable light
	//g_pDevice->LightEnable(0, TRUE);
}

/*Turns the light on by calling the LightEnable function

g_pDevice - the direct3d device used for rendering
*/
void PointLight::lightOn(LPDIRECT3DDEVICE9 g_pDevice)
{
	g_pDevice->LightEnable(1, true);
}

/*Turns the light off by calling the LightEnable function

g_pDevice - the direct3d device used for rendering
*/
void PointLight::lightOff(LPDIRECT3DDEVICE9 g_pDevice)
{
	g_pDevice->LightEnable(1, false);
}
