#pragma once

#include "AABB.h"

class Collidable : public AABB
{
public:
	Collidable(const AABB &bounds);
	Collidable(float x, float y, float width, float height);
	virtual ~Collidable() {}

	bool isCollidingWith(const Collidable &other) const;
	bool contains(float x, float y) const;
	bool contains(int x, int y) const;
};