#pragma once

#include "DriverState.h"
#include "Listener.h"
#include "DriverStateType.h"
#include "EventQueue.h"

class Driver :
	public DriverState,
	public Listener
{
private:
	DriverState *state_;
	EventQueue eventQueue_;

	bool isDone_;

	void changeState_(DriverStateType toStateType);
public:
	Driver();
	~Driver();

	void onEvent(const Event &ev) override;

	void updateOnTick() override;
	void updateOnMousePress(unsigned int allegroMouseButton, int mouseX, int mouseY) override;
	void updateOnKeyPress(int allegroKeycode) override;
	void updateOnKeyRelease(int allegroKeycode) override;

	void render() const override;

	bool isDone() const;

	EventQueue &eventQueue();
};