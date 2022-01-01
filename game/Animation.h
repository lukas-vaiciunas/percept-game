#pragma once

#include <vector>
#include "Image.h"
#include "TickAlarm.h"

class Animation
{
private:
	std::vector<Image> frames_;
	TickAlarm frameAlarm_;

	unsigned int currentFrameIndex_;
public:
	Animation(const std::vector<Image> &frames, unsigned int numFrameTicks);

	void updateOnTick();

	const Image &currentFrame() const;
};