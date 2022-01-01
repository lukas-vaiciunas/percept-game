#pragma once

class CollidableTile;

#include <vector>
#include "Collidable.h"
#include "TickAlarm.h"
#include "Image.h"

class Bullet : public Collidable
{
private:
	TickAlarm tickAlarm_;
	Image image_;
	float velX_;
	float velY_;
	float angleRad_;
	unsigned int damage_;
	bool isLive_;
	bool isFromPlayer_;

	void updateTileCollisions_(const std::vector<CollidableTile *> &possibleCollidableTiles);
public:
	Bullet(
		float x, float y,
		float velX, float velY,
		unsigned int damage,
		const Image &image,
		unsigned int numLifeTicks,
		bool isFromPlayer);

	void updateOnTick(const std::vector<CollidableTile *> &possibleCollidableTiles);
	void render() const;

	float angleRad() const;
	unsigned int damage() const;
	bool isLive() const;
	bool isFromPlayer() const;
};