#include "Entity.h"
#include "ParticlePool.h"

Entity::Entity(float x, float y, float width, float height, float speed, unsigned int health, unsigned int damage) :
	Collidable(x, y, width, height),
	speed_(speed),
	health_(health),
	maxHealth_(health),
	damage_(damage),
	isLive_(true)
{}

void Entity::heal(unsigned int health)
{
	health_ += health;
	if (health_ > maxHealth_)
		health_ = maxHealth_;
}

unsigned int Entity::health() const
{
	return health_;
}

unsigned int Entity::maxHealth() const
{
	return maxHealth_;
}

unsigned int Entity::damage() const
{
	return damage_;
}

bool Entity::isLive() const
{
	return isLive_;
}

void Entity::hurt_(unsigned int damage)
{
	health_ -= damage;
}

void Entity::onHurt_(ParticlePool &particlePool)
{
	particlePool.spawnHurtGibs(
		this->x() + this->width() * 0.5f,
		this->y() + this->height() * 0.5f,
		this->y() + this->height());
}

void Entity::onDeath_(ParticlePool &particlePool)
{
	health_ = 0;
	isLive_ = false;

	particlePool.spawnDeathGibs(
		this->x() + this->width() * 0.5f,
		this->y() + this->height() * 0.5f,
		this->y() + this->height());
}

bool Entity::isFatal_(unsigned int damage) const
{
	return damage >= health_;
}

float Entity::getSpeed_() const
{
	return speed_;
}

void Entity::addMaxHealth_(unsigned int amount)
{
	maxHealth_ += amount;
}

void Entity::subMaxHealth_(unsigned int amount)
{
	maxHealth_ -= amount;
	if (health_ > maxHealth_)
		health_ = maxHealth_;
}

void Entity::setMaxHealth_(unsigned int amount)
{
	maxHealth_ = amount;
	if (health_ > maxHealth_)
		health_ = maxHealth_;
}

void Entity::addDamage_(unsigned int amount)
{
	damage_ += amount;
}

void Entity::subDamage_(unsigned int amount)
{
	damage_ -= amount;
}

void Entity::setDamage_(unsigned int amount)
{
	damage_ = amount;
}