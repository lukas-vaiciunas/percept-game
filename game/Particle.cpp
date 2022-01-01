#include "Particle.h"

Particle::Particle(
	float x, float y,
	float velX, float velY,
	float angleRad,
	const Image &image,
	unsigned int numLifeTicks)
	:
	AABB(x, y, image.width(), image.height()),
	image_(image),
	lifeAlarm_(numLifeTicks),
	velX_(velX),
	velY_(velY),
	angleRad_(angleRad),
	isLive_(true)
{}

void Particle::updateOnTick()
{
	this->moveX_(velX_);
	this->moveY_(velY_);

	lifeAlarm_.updateOnTick();
	if (lifeAlarm_.isAlarm())
		isLive_ = false;
}

void Particle::render() const
{
	image_.renderRotated(this->x(), this->y(), angleRad_, true);
}

bool Particle::isLive() const
{
	return isLive_;
}

void Particle::setVelX_(float velX)
{
	velX_ = velX;
}

void Particle::setVelY_(float velY)
{
	velY_ = velY;
}

void Particle::accelerateY_(float accY)
{
	velY_ += accY;
}

void Particle::spin_(float dAngleRad)
{
	angleRad_ += dAngleRad;
}