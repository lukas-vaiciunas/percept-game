#pragma once

class EventQueue;

#include <string>
#include "Image.h"

class Item
{
private:
	const std::string statusString_;
	Image image_;
public:
	Item(const std::string &statusString, const Image &image);
	virtual ~Item() {}

	virtual Item *clone() const = 0;

	virtual void use(EventQueue &gameEventQueue) const = 0;

	const std::string &statusString() const;
	const Image &image() const;
};