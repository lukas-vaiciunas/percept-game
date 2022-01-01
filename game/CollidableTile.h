#pragma once

#include "Tile.h"
#include "Collidable.h"

class CollidableTile :
	public Tile,
	public Collidable
{
public:
	CollidableTile(float x, float y, unsigned int imageId, const ImageData &imageData);

	void render(const ImageData &imageData) const override;
	void renderTinted(const ImageData &imageData, const ALLEGRO_COLOR &tint) const override;
};