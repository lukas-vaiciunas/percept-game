#include "Listener.h"
#include "EventQueue.h"

Listener::Listener(const std::unordered_set<EventType> &acceptedEventTypes) :
	acceptedEventTypes_(acceptedEventTypes)
{}

bool Listener::acceptsEventType(EventType eventType) const
{
	return acceptedEventTypes_.find(eventType) != acceptedEventTypes_.cend();
}