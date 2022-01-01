#include <cmath>
#include "BulletPool.h"

BulletPool::BulletPool(unsigned int capacity) :
	Pool(capacity)
{}

void BulletPool::updateOnTick(float cX, float cY, float viewRadius,
	const std::vector<CollidableTile *> &possibleCollidableTiles)
{
	visibleIndices_.clear();

	for (unsigned int i = 0; i < this->numLive(); )
	{
		Bullet *const target = this->object_(i);

		target->updateOnTick(possibleCollidableTiles);

		if (!target->isLive())
		{
			this->removeAtIndex_(i);
		}
		else
		{
			float distanceToBullet = std::sqrtf(
				std::powf(target->x() + target->width() * 0.5f - cX, 2.0f)
				+ std::powf(target->y() + target->height() * 0.5f - cY, 2.0f));

			if (distanceToBullet <= viewRadius)
				visibleIndices_.push_back(i);

			++i;
		}
	}
}

void BulletPool::render() const
{
	// for (auto it = this->cbegin(); it != this->cend(); ++it)
	// 	(*it)->render();

	for (const unsigned int &i : visibleIndices_)
		this->object_(i)->render();
}