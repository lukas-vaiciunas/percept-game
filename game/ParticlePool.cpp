#include <random>
#include "ParticlePool.h"
#include "BounceParticle.h"

ParticlePool::ParticlePool(unsigned int capacity) :
	Pool(capacity),
	visibleIndices_({})
{}

void ParticlePool::updateOnTick(float cX, float cY, float viewRadius)
{
	visibleIndices_.clear();

	for (unsigned int i = 0; i < this->numLive(); )
	{
		Particle *const target = this->object_(i);

		target->updateOnTick();

		if (!target->isLive())
		{
			this->removeAtIndex_(i);
		}
		else
		{
			float distanceToParticle = std::sqrtf(
				std::powf(target->x() + target->width() * 0.5f - cX, 2.0f)
				+ std::powf(target->y() + target->height() * 0.5f - cY, 2.0f));

			if (distanceToParticle <= viewRadius)
				visibleIndices_.push_back(i);

			++i;
		}
	}
}

void ParticlePool::spawnDeathGibs(float x, float y, float maxY)
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<float> velXDis(-1.0f, 1.0f);
	std::uniform_real_distribution<float> velYDis(-3.0f, -2.0f);
	std::uniform_real_distribution<float> dAngleRadDis(-0.1f, 0.1f);

	// if (!this->full()) this->add(new BounceParticle(x, y, y, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-1.png"), 240));
	// if (!this->full()) this->add(new BounceParticle(x, y, y, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-2.png"), 240));
	// if (!this->full()) this->add(new BounceParticle(x, y, y, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-3.png"), 240));
	// if (!this->full()) this->add(new BounceParticle(x, y, y, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-4.png"), 240));
	// if (!this->full()) this->add(new BounceParticle(x, y, y, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-5.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/bone.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/bone.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/bone.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/skull.png"), 240));
}

void ParticlePool::spawnHurtGibs(float x, float y, float maxY)
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<float> velXDis(-1.0f, 1.0f);
	std::uniform_real_distribution<float> velYDis(-3.0f, -2.0f);
	std::uniform_real_distribution<float> dAngleRadDis(-0.1f, 0.1f);

	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-1.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-2.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-3.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-4.png"), 240));
	if (!this->full()) this->add(new BounceParticle(x, y, maxY, velXDis(eng), velYDis(eng), 0.15f, 0.0f, dAngleRadDis(eng), Image("img/blood-5.png"), 240));
}

void ParticlePool::render() const
{
	// for (auto it = this->cbegin(); it != this->cend(); ++it)
	// 	(*it)->render();

	for (const unsigned int &i : visibleIndices_)
		this->object_(i)->render();
}