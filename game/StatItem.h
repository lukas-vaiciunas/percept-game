#pragma once

#include "Item.h"
#include "StatMod.h"

class StatItem : public Item
{
private:
	StatMod::Stat stat_;
	StatMod::Operator op_;
	unsigned int amount_;
public:
	StatItem(StatMod::Stat stat, StatMod::Operator op, unsigned int amount, const std::string &statusString, const Image &image);

	StatItem *clone() const override;

	void use(EventQueue &gameEventQueue) const override;
};