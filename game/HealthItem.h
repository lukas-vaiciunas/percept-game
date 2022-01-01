#pragma once

#include "Item.h"

class HealthItem : public Item
{
private:
	unsigned int health_;
public:
	HealthItem(unsigned int health, const std::string &statusString, const Image &image);

	HealthItem *clone() const override;

	void use(EventQueue &gameEventQueue) const override;
};