#include "AABB.h"

AABB::AABB(float x, float y, float width, float height) :
	x_(x),
	y_(y),
	width_(width),
	height_(height)
{}

float AABB::x() const
{
	return x_;
}

float AABB::y() const
{
	return y_;
}

float AABB::width() const
{
	return width_;
}

float AABB::height() const
{
	return height_;
}

void AABB::setX_(float x)
{
	x_ = x;
}

void AABB::setY_(float y)
{
	y_ = y;
}

void AABB::moveX_(float dX)
{
	x_ += dX;
}

void AABB::moveY_(float dY)
{
	y_ += dY;
}