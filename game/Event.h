#pragma once

#include "EventType.h"

class Event
{
private:
	EventType type_;
public:
	Event(EventType type) :
		type_(type)
	{}
	virtual ~Event() {}

	inline EventType type() const
	{
		return type_;
	}
};