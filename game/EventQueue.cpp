#include "EventQueue.h"
#include "Event.h"
#include "Listener.h"

EventQueue::~EventQueue()
{
	while (!events_.empty())
	{
		delete events_.front();
		events_.pop();
	}
}

/*EventQueue &EventQueue::instance()
{
	static EventQueue eventQueue;
	return eventQueue;
}*/

void EventQueue::dispatch()
{
	if (events_.empty())
		return;

	const Event &nextEvent = *events_.front();

	for (Listener *const listener : listeners_)
		if (listener->acceptsEventType(nextEvent.type()))
			listener->onEvent(nextEvent);

	delete events_.front();
	events_.pop();
}

void EventQueue::send(Event *ev)
{
	events_.push(ev);
}

void EventQueue::attach(Listener *listener)
{
	listeners_.push_back(listener);
}