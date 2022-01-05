#pragma once

class EventQueue;

#include <string>
#include "Collidable.h"
#include "Cloneable.h"
#include "Image.h"

class Door :
	public Collidable,
	public Cloneable
{
private:
	const std::string levelPath_;

	Image image_;

	bool isDiscovered_;
public:
	Door(float x, float y, const std::string &levelPath, const Image &image);

	Door *clone() const override;

	void render() const;
	void renderTinted(const ALLEGRO_COLOR &tint) const;

	void open(EventQueue &gameEventQueue) const;

	void discover();

	bool isDiscovered() const;
};