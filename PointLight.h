#pragma once

#include "basics.h"

class PointLight
{
public:
	PointLight();
	~PointLight();

	void InitLight(LPDIRECT3DDEVICE9 g_pDevice);

	void lightOn(LPDIRECT3DDEVICE9 g_pDevice);
	void lightOff(LPDIRECT3DDEVICE9 g_pDevice);

private:
	bool bLight;
};