#pragma once

class TickAlarm
{
private:
	unsigned int targetNumTicks_;
	unsigned int numTicks_;
public:
	TickAlarm(unsigned int targetNumTicks);

	void updateOnTick();

	void reset();
	void setTargetNumTicks(unsigned int targetNumTicks);

	unsigned int targetNumTicks() const;
	unsigned int numTicks() const;
	bool isAlarm() const;
};