#include "StatItem.h"
#include "EventQueue.h"
#include "GameEvents.h"

StatItem::StatItem(StatMod::Stat stat, StatMod::Operator op, unsigned int amount, const std::string &statusString, const Image &image) :
	Item(statusString, image),
	stat_(stat),
	op_(op),
	amount_(amount)
{}

StatItem *StatItem::clone() const
{
	return new StatItem(stat_, op_, amount_, this->statusString(), this->image());
}

void StatItem::use(EventQueue &gameEventQueue) const
{
	gameEventQueue.send(new EventChangePlayerStats(stat_, op_, amount_));
}