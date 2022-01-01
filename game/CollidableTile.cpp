#include "CollidableTile.h"
#include "ImageData.h"

CollidableTile::CollidableTile(float x, float y, unsigned int imageId, const ImageData &imageData) :
	Tile(imageId),
	Collidable(x, y, imageData.getImage(imageId).width(), imageData.getImage(imageId).height())
{}

void CollidableTile::render(const ImageData &imageData) const
{
	this->render_(this->x(), this->y(), imageData);
}

void CollidableTile::renderTinted(const ImageData &imageData, const ALLEGRO_COLOR &tint) const
{
	this->renderTinted_(this->x(), this->y(), tint, imageData);
}