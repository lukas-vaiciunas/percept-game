#include "TickAlarm.h"

TickAlarm::TickAlarm(unsigned int targetNumTicks) :
	targetNumTicks_(targetNumTicks),
	numTicks_(0)
{}

void TickAlarm::updateOnTick()
{
	++numTicks_;
}

void TickAlarm::reset()
{
	numTicks_ = 0;
}

void TickAlarm::setTargetNumTicks(unsigned int targetNumTicks)
{
	targetNumTicks_ = targetNumTicks;
}

unsigned int TickAlarm::targetNumTicks() const
{
	return targetNumTicks_;
}

unsigned int TickAlarm::numTicks() const
{
	return numTicks_;
}

bool TickAlarm::isAlarm() const
{
	return numTicks_ >= targetNumTicks_;
}