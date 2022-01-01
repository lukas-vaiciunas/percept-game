#pragma once

#include "Pool.h"
#include "Particle.h"

class ParticlePool : public Pool<Particle>
{
private:
	std::vector<unsigned int> visibleIndices_;
public:
	ParticlePool(unsigned int capacity);

	void updateOnTick(float cX, float cY, float viewRadius);

	void spawnDeathGibs(float x, float y, float maxY);
	void spawnHurtGibs(float x, float y, float maxY);

	void render() const;
};