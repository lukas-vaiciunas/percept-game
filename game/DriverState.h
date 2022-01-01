#pragma once

class DriverState
{
public:
	DriverState() {}
	virtual ~DriverState() {}

	virtual void updateOnTick() {}
	virtual void updateOnMousePress(unsigned int allegroMouseButton, int mouseX, int mouseY) {}
	virtual void updateOnKeyPress(int allegroKeycode) {}
	virtual void updateOnKeyRelease(int allegroKeycode) {}

	virtual void render() const {}
};