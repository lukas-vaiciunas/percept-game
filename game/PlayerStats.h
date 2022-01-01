#pragma once

class PlayerStats
{
private:
	unsigned int health_;
	unsigned int maxHealth_;
	unsigned int damage_;
public:
	PlayerStats(unsigned int maxHealth, unsigned int damage);

	void addHealth(unsigned int amount);
	void subHealth(unsigned int amount);

	void addMaxHealth(unsigned int amount);
	void subMaxHealth(unsigned int amount);

	void addDamage(unsigned int amount);
	void subDamage(unsigned int amount);

	unsigned int health() const;
	unsigned int maxHealth() const;
	unsigned int damage() const;
};