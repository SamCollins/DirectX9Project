#pragma once

#include "basics.h"

class SpotLight
{
public:
	SpotLight();
	~SpotLight();

	void InitLight(LPDIRECT3DDEVICE9 g_pDevice);

	void lightOn(LPDIRECT3DDEVICE9 g_pDevice);
	void lightOff(LPDIRECT3DDEVICE9 g_pDevice);

private:
	bool bLight;
};