#include "Driver.h"
#include "MenuDriverState.h"
#include "GameDriverState.h"
#include "DriverEvents.h"

Driver::Driver() :
	DriverState(),
	Listener({ EventType::ChangeDriverState, EventType::Quit }),
	eventQueue_(),
	state_(nullptr),
	isDone_(false)
{
	eventQueue_.attach(this);

	this->changeState_(DriverStateType::Menu);
}

Driver::~Driver()
{
	this->changeState_(DriverStateType::None);
}

void Driver::onEvent(const Event &ev)
{
	switch (ev.type())
	{
	case EventType::ChangeDriverState:
	{
		const EventChangeDriverState &trueEv = static_cast<const EventChangeDriverState &>(ev);
		this->changeState_(trueEv.driverStateType());
		break;
	}
	case EventType::Quit:
		isDone_ = true;
		break;
	}
}

void Driver::updateOnTick()
{
	eventQueue_.dispatch();
	state_->updateOnTick();
}

void Driver::updateOnMousePress(unsigned int allegroMouseButton, int mouseX, int mouseY)
{
	state_->updateOnMousePress(allegroMouseButton, mouseX, mouseY);
}

void Driver::updateOnKeyPress(int allegroKeycode)
{
	state_->updateOnKeyPress(allegroKeycode);
}

void Driver::updateOnKeyRelease(int allegroKeycode)
{
	state_->updateOnKeyRelease(allegroKeycode);
}

void Driver::render() const
{
	state_->render();
}

bool Driver::isDone() const
{
	return isDone_;
}

EventQueue &Driver::eventQueue()
{
	return eventQueue_;
}

void Driver::changeState_(DriverStateType toStateType)
{
	delete state_;

	switch (toStateType)
	{
	case DriverStateType::None:
		break;
	case DriverStateType::Menu:
		state_ = new MenuDriverState(eventQueue_);
		break;
	case DriverStateType::Game:
		state_ = new GameDriverState(eventQueue_);
		break;
	}
}