#pragma once

class EventQueue;

#include "DriverState.h"
#include "Button.h"

class MenuDriverState : public DriverState
{
private:
	EventQueue &driverEventQueue_;

	Button startButton_;
	Button quitButton_;
public:
	MenuDriverState(EventQueue &driverEventQueue);

	void updateOnMousePress(unsigned int allegroMouseButton, int mouseX, int mouseY) override;

	void render() const override;
};