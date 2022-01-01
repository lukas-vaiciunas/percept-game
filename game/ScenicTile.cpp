#include "ScenicTile.h"

ScenicTile::ScenicTile(float x, float y, float width, float height, unsigned int imageId) :
	Tile(imageId),
	x_(x),
	y_(y),
	width_(width),
	height_(height)
{}

void ScenicTile::render(const ImageData &imageData) const
{
	this->render_(x_, y_, imageData);
}

void ScenicTile::renderTinted(const ImageData &imageData, const ALLEGRO_COLOR &tint) const
{
	this->renderTinted_(x_, y_, tint, imageData);
}

float ScenicTile::x() const
{
	return x_;
}

float ScenicTile::y() const
{
	return y_;
}
float ScenicTile::width() const
{
	return width_;
}

float ScenicTile::height() const
{
	return height_;
}