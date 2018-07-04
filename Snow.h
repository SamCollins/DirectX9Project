#pragma once
#include "PSystem.h"

class Snow : public PSystem
{
public:
	Snow(int numParticles);
	void resetParticle(Attribute* attribute);
	void update(float timeDelta);
};
