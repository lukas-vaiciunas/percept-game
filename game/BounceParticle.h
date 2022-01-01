#pragma once

#include "Particle.h"

class BounceParticle : public Particle
{
private:
	float bounceY_;
	float accY_;
	float dAngleRad_;

	bool isMoving_;
	bool didBounce_;
public:
	BounceParticle(
		float x, float y, float bounceY,
		float velX, float velY,
		float accY,
		float angleRad,
		float dAngleRad,
		const Image &image,
		unsigned int numLifeTicks);

	void updateOnTick() override;
};