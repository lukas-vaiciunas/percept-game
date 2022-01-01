#pragma once

#include "Image.h"

class HealthDisplay
{
private:
	Image iconHeartFull_;
	Image iconHeartHalf_;
	Image iconHeartEmpty_;

	float x_;
	float y_;

	unsigned int numFullHearts_;
	unsigned int numHalfHearts_;
	unsigned int numEmptyHearts_;
public:
	HealthDisplay(float x, float y);

	void update(int health, int maxHealth);

	void render() const;
};