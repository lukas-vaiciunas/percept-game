#include "Door.h"
#include "EventQueue.h"
#include "GameEvents.h"

Door::Door(float x, float y, const std::string &levelPath, const Image &image) :
	Collidable(x, y, image.width(), image.height()),
	levelPath_(levelPath),
	image_(image),
	isDiscovered_(false)
{}

Door *Door::clone() const
{
	return new Door(*this);
}

void Door::render() const
{
	image_.render(this->x(), this->y());
}

void Door::renderTinted(const ALLEGRO_COLOR &tint) const
{
	image_.renderTinted(this->x(), this->y(), tint);
}

void Door::open(EventQueue &gameEventQueue) const
{
	gameEventQueue.send(new EventChangeLevel(levelPath_));
}

void Door::discover()
{
	isDiscovered_ = true;
}

bool Door::isDiscovered() const
{
	return isDiscovered_;
}