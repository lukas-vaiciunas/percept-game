#include "BounceParticle.h"

BounceParticle::BounceParticle(
	float x, float y, float bounceY,
	float velX, float velY,
	float accY,
	float angleRad,
	float dAngleRad,
	const Image &image,
	unsigned int numLifeTicks)
	:
	Particle(x, y, velX, velY, angleRad, image, numLifeTicks),
	bounceY_(bounceY),
	accY_(accY),
	dAngleRad_(dAngleRad),
	isMoving_(true),
	didBounce_(false)
{}

void BounceParticle::updateOnTick()
{
	Particle::updateOnTick();

	if (!isMoving_)
		return;

	this->spin_(dAngleRad_);

	if (this->y() >= bounceY_)
	{
		this->setY_(bounceY_);

		if (!didBounce_)
		{
			didBounce_ = true;
			this->setVelY_(-1.0f);
		}
		else
		{
			this->setVelX_(0.0f);
			this->setVelY_(0.0f);
			isMoving_ = false;
		}
	}
	else this->accelerateY_(accY_);
}