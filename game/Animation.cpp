#include "Animation.h"

Animation::Animation(const std::vector<Image> &frames, unsigned int numFrameTicks) :
	frames_(frames),
	frameAlarm_(numFrameTicks),
	currentFrameIndex_(0)
{}

void Animation::updateOnTick()
{
	frameAlarm_.updateOnTick();
	if (frameAlarm_.isAlarm())
	{
		frameAlarm_.reset();
		currentFrameIndex_ = (currentFrameIndex_ + 1) % frames_.size();
	}
}

const Image &Animation::currentFrame() const
{
	return frames_.at(currentFrameIndex_);
}