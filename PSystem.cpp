#include <cstdlib>
#include "pSystem.h"

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

PSystem::PSystem()
	: _device(0)
	, _vb(0)
	, _tex(0)
{
}

PSystem::~PSystem()
{
	_vb->Release();
	_tex->Release();
}

bool PSystem::init(IDirect3DDevice9* device, char* texFileName)
{
	_device = device; // save a ptr to the device

	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
		_vbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&_vb,
		0);

	if (FAILED(hr))
	{
		::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		device,
		texFileName,
		&_tex);

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateTextureFromFile() - FAILED", "PSystem", 0);
		return false;
	}

	return true;
}

void PSystem::reset()
{
	list<Attribute>::iterator i;
	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		resetParticle(&(*i));
	}
}

void PSystem::addParticle()
{
	Attribute attribute;
	resetParticle(&attribute);
	_particles.push_back(attribute);
}

void PSystem::preRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, false);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true);
	_device->SetRenderState(D3DRS_POINTSIZE, FtoDw(_size));
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

	// control the size of the particle relative to distance
	_device->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

	// use alpha from texture
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void PSystem::postRender()
{
	_device->SetRenderState(D3DRS_LIGHTING, true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void PSystem::render()
{
	if (!_particles.empty())
	{
		// set render states
		preRender();

		_device->SetTexture(0, _tex);
		_device->SetFVF(Particle::FVF);
		_device->SetStreamSource(0, _vb, 0, sizeof(Particle));

		// render batches one by one
		// start at beginning if we're at the end of the vb
		if (_vbOffset >= _vbSize)
			_vbOffset = 0;

		Particle* v = 0;

		_vb->Lock(_vbOffset * sizeof(Particle), _vbBatchSize * sizeof(Particle), (void**)&v, 
			_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		// Until all particles have been rendered.
		std::list<Attribute>::iterator i;
		for (i = _particles.begin(); i != _particles.end(); i++)
		{
			if (i->_isAlive)
			{
				// Copy a batch of the living particles to the next vertex buffer segment
				v->_position = i->_position;
				v->_color = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; //increase batch counter

				// if this batch full?
				if (numParticlesInBatch == _vbBatchSize)
				{
					// Draw the last batch of particles that was copied to the vertex buffer. 
					_vb->Unlock();

					_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, _vbBatchSize);

					// While that batch is drawing, start filling the next batch with particles.
					
					// move the offset to the start of the next batch
					_vbOffset += _vbBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					if (_vbOffset >= _vbSize)
						_vbOffset = 0;

					_vb->Lock(_vbOffset * sizeof(Particle), _vbBatchSize * sizeof(Particle), (void**)&v,
						_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}
			}
		}

		_vb->Unlock();

		// its possible that the LAST batch being filled never got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have been satisfied.  We draw the last partially filled batch now.

		if (numParticlesInBatch)
		{
			_device->DrawPrimitive(D3DPT_POINTLIST, _vbOffset, numParticlesInBatch);
		}

		// next block
		_vbOffset += _vbBatchSize;

		// reset render states
		postRender();
	}
}

bool PSystem::isEmpty()
{
	return _particles.empty();
}

bool PSystem::isDead()
{
	list<Attribute>::iterator i;
	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		// If any particles still alive, the system is not dead
		if (i->_isAlive)
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}

void PSystem::removeDeadParticles()
{
	list<Attribute>::iterator i = _particles.begin();

	while (i != _particles.end())
	{
		if (i->_isAlive == false)
		{
			// erase returns the next iterator, so no need to
			// incrememnt to the next one ourselves.
			i = _particles.erase(i);
		}
		else
		{
			i++; // next in list
		}
	}
}


#pragma region Utility

BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = INFINITY;
	_min.y = INFINITY;
	_min.z = INFINITY;

	_max.x = -INFINITY;
	_max.y = -INFINITY;
	_max.z = -INFINITY;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float PSystem::GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f;

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound;
}

void PSystem::GetRandomVector(
	D3DXVECTOR3* out,
	D3DXVECTOR3* min,
	D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD PSystem::FtoDw(float f)
{
	return *((DWORD*)&f);
}

#pragma endregion