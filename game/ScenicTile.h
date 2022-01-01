#pragma once

#include "Tile.h"

class ScenicTile : public Tile
{
private:
	float x_;
	float y_;
	float width_;
	float height_;
public:
	ScenicTile(float x, float y, float width, float height, unsigned int imageId);

	void render(const ImageData &imageData) const override;
	void renderTinted(const ImageData &imageData, const ALLEGRO_COLOR &tint) const override;

	float x() const;
	float y() const;
	float width() const;
	float height() const;
};