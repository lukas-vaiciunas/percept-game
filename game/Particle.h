#pragma once

#include "AABB.h"
#include "Image.h"
#include "TickAlarm.h"

class Particle : public AABB
{
private:
	const Image image_;
	TickAlarm lifeAlarm_;

	float velX_;
	float velY_;
	float angleRad_;

	bool isLive_;
protected:
	void setVelX_(float velX);
	void setVelY_(float velY);
	void accelerateY_(float accY);
	void spin_(float dAngleRad);
public:
	Particle(
		float x, float y,
		float velX, float velY,
		float angleRad,
		const Image &image,
		unsigned int numLifeTicks);

	virtual ~Particle() {}

	virtual void updateOnTick();

	void render() const;

	bool isLive() const;
};