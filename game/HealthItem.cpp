#include "HealthItem.h"
#include "EventQueue.h"
#include "GameEvents.h"

HealthItem::HealthItem(unsigned int health, const std::string &statusString, const Image &image) :
	Item(statusString, image),
	health_(health)
{}

HealthItem *HealthItem::clone() const
{
	return new HealthItem(health_, this->statusString(), this->image());
}

void HealthItem::use(EventQueue &gameEventQueue) const
{
	gameEventQueue.send(new EventHealPlayer(health_));
}