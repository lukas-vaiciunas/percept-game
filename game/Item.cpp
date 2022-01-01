#include "Item.h"

Item::Item(const std::string &statusString, const Image &image) :
	statusString_(statusString),
	image_(image)
{}

const std::string &Item::statusString() const
{
	return statusString_;
}

const Image &Item::image() const
{
	return image_;
}