#include "Tile.h"
#include "ImageData.h"

Tile::Tile(unsigned int imageId) :
	imageId_(imageId),
	isDiscovered_(false)
{}

void Tile::discover()
{
	isDiscovered_ = true;
}

bool Tile::isDiscovered() const
{
	return isDiscovered_;
}

void Tile::render_(float x, float y, const ImageData &imageData) const
{
	imageData.getImage(imageId_).render(x, y);
}

void Tile::renderTinted_(float x, float y, const ALLEGRO_COLOR &tint, const ImageData &imageData) const
{
	imageData.getImage(imageId_).renderTinted(x, y, tint);
}