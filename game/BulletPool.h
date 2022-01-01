#pragma once

#include "Pool.h"
#include "Bullet.h"

class BulletPool : public Pool<Bullet>
{
private:
	std::vector<unsigned int> visibleIndices_;
public:
	BulletPool(unsigned int capacity);

	void updateOnTick(float cX, float cY, float viewRadius,
		const std::vector<CollidableTile *> &possibleCollidableTiles);

	void render() const;
};