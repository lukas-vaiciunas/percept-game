#pragma once

class Event;
class Listener;

#include <queue>
#include <vector>

class EventQueue
{
private:
	std::queue<Event *> events_;
	std::vector<Listener *> listeners_;

	//EventQueue() {}
	//~EventQueue();
public:
	EventQueue() {}
	~EventQueue();
	//EventQueue(const EventQueue &) = delete;
	//EventQueue &operator=(const EventQueue &) = delete;

	//static EventQueue &instance();

	void dispatch();
	void send(Event *ev);

	void attach(Listener *listener);
};