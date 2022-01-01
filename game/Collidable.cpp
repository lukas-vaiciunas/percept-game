#include "Collidable.h"

Collidable::Collidable(const AABB &bounds) :
	AABB(bounds)
{}

Collidable::Collidable(float x, float y, float width, float height) :
	AABB(x, y, width, height)
{}

bool Collidable::isCollidingWith(const Collidable &other) const
{
	return this->x() < other.x() + other.width()
		&& this->x() + this->width() > other.x()
		&& this->y() < other.y() + other.height()
		&& this->y() + this->height() > other.y();
}

bool Collidable::contains(float x, float y) const
{
	return this->x() < x
		&& this->x() + this->width() > x
		&& this->y() < y
		&& this->y() + this->height() > y;
}

bool Collidable::contains(int x, int y) const
{
	return this->x() < x
		&& this->x() + this->width() > x
		&& this->y() < y
		&& this->y() + this->height() > y;
}