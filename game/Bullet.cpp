//#include <allegro5/allegro_primitives.h>
#include <cmath>
#include "Bullet.h"
#include "CollidableTile.h"

Bullet::Bullet(
	float x, float y,
	float velX, float velY,
	unsigned int damage,
	const Image &image,
	unsigned int numLifeTicks,
	bool isFromPlayer)
	:
	Collidable(x, y, image.width(), image.height()),
	tickAlarm_(numLifeTicks),
	image_(image),
	velX_(velX),
	velY_(velY),
	angleRad_(std::atan2f(velY, velX)),
	damage_(damage),
	isLive_(true),
	isFromPlayer_(isFromPlayer)
{}

void Bullet::updateOnTick(const std::vector<CollidableTile *> &possibleCollidableTiles)
{
	this->moveX_(velX_);
	this->moveY_(velY_);

	this->updateTileCollisions_(possibleCollidableTiles);

	tickAlarm_.updateOnTick();
	if (tickAlarm_.isAlarm())
		isLive_ = false;
}

void Bullet::render() const
{
	image_.renderRotated(this->x(), this->y(), angleRad_, true);

	//al_draw_filled_rectangle(this->x(), this->y(), this->x() + this->width(), this->y() + this->height(), al_map_rgba_f(0.0f, 1.0f, 1.0f, 0.25f));
}

float Bullet::angleRad() const
{
	return angleRad_;
}

unsigned int Bullet::damage() const
{
	return damage_;
}

bool Bullet::isLive() const
{
	return isLive_;
}

bool Bullet::isFromPlayer() const
{
	return isFromPlayer_;
}

void Bullet::updateTileCollisions_(const std::vector<CollidableTile *> &possibleCollidableTiles)
{
	for (const CollidableTile *const tile : possibleCollidableTiles)
	{
		if (this->isCollidingWith(*tile))
		{
			isLive_ = false;
			break;
		}
	}
}