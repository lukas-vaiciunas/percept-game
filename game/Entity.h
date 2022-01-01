#pragma once

class ParticlePool;

#include "Collidable.h"

class Entity : public Collidable
{
private:
	float speed_;
	unsigned int health_;
	unsigned int maxHealth_;
	unsigned int damage_;

	bool isLive_;
protected:
	void hurt_(unsigned int damage);
	void onHurt_(ParticlePool &particlePool);
	void onDeath_(ParticlePool &particlePool);
	bool isFatal_(unsigned int damage) const;
	float getSpeed_() const;

	void addMaxHealth_(unsigned int amount);
	void subMaxHealth_(unsigned int amount);
	void setMaxHealth_(unsigned int amount);

	void addDamage_(unsigned int amount);
	void subDamage_(unsigned int amount);
	void setDamage_(unsigned int amount);
public:
	Entity(float x, float y, float width, float height, float speed, unsigned int health, unsigned int damage);
	virtual ~Entity() {}

	void heal(unsigned int health);

	virtual void render() const = 0;

	unsigned int health() const;
	unsigned int maxHealth() const;
	unsigned int damage() const;
	bool isLive() const;
};