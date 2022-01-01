#pragma once

#include "Event.h"
#include "DriverStateType.h"

class EventChangeDriverState : public Event
{
private:
	DriverStateType driverStateType_;
public:
	EventChangeDriverState(DriverStateType driverStateType) :
		Event(EventType::ChangeDriverState),
		driverStateType_(driverStateType)
	{}

	inline DriverStateType driverStateType() const
	{
		return driverStateType_;
	}
};

class EventQuit : public Event
{
public:
	EventQuit() :
		Event(EventType::Quit)
	{}
};