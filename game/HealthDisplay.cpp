#include "HealthDisplay.h"

HealthDisplay::HealthDisplay(float x, float y) :
	iconHeartFull_("img/icon-heart-full.png"),
	iconHeartHalf_("img/icon-heart-half.png"),
	iconHeartEmpty_("img/icon-heart-empty.png"),
	x_(x),
	y_(y),
	numFullHearts_(0),
	numHalfHearts_(0),
	numEmptyHearts_(0)
{}

void HealthDisplay::update(int health, int maxHealth)
{
	numFullHearts_ = health / 2;
	numHalfHearts_ = health % 2;
	numEmptyHearts_ = (maxHealth - health) / 2;
}

void HealthDisplay::render() const
{
	for (unsigned int i = 0; i < numFullHearts_; ++i)
		iconHeartFull_.render(x_ + i * 9.0f, y_);

	for (unsigned int i = 0; i < numHalfHearts_; ++i)
		iconHeartHalf_.render(x_ + (numFullHearts_ + i) * 9.0f, y_);

	for (unsigned int i = 0; i < numEmptyHearts_; ++i)
		iconHeartEmpty_.render(x_ + (numFullHearts_ + numHalfHearts_ + i) * 9.0f, y_);
}