#include "PlayerStats.h"

PlayerStats::PlayerStats(unsigned int maxHealth, unsigned int damage) :
	health_(maxHealth),
	maxHealth_(maxHealth),
	damage_(damage)
{}

void PlayerStats::addHealth(unsigned int amount)
{
	health_ += amount;
}

void PlayerStats::subHealth(unsigned int amount)
{
	health_ -= amount;
}

void PlayerStats::addMaxHealth(unsigned int amount)
{
	maxHealth_ += amount;
}

void PlayerStats::subMaxHealth(unsigned int amount)
{
	maxHealth_ -= amount;
}

void PlayerStats::addDamage(unsigned int amount)
{
	damage_ += amount;
}

void PlayerStats::subDamage(unsigned int amount)
{
	damage_ -= amount;
}

unsigned int PlayerStats::health() const
{
	return health_;
}

unsigned int PlayerStats::maxHealth() const
{
	return maxHealth_;
}

unsigned int PlayerStats::damage() const
{
	return damage_;
}