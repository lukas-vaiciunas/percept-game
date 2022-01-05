#pragma once

class ImageData;
struct ALLEGRO_COLOR;

#include "Cloneable.h"

class Tile : public Cloneable
{
private:
	unsigned int imageId_;
	bool isDiscovered_;
protected:
	void render_(float x, float y, const ImageData &imageData) const;
	void renderTinted_(float x, float y, const ALLEGRO_COLOR &tint, const ImageData &imageData) const;
public:
	Tile(unsigned int imageId);
	virtual ~Tile() {}

	virtual Tile *clone() const override = 0;

	void discover();

	virtual void render(const ImageData &imageData) const = 0;
	virtual void renderTinted(const ImageData &imageData, const ALLEGRO_COLOR &tint) const = 0;

	bool isDiscovered() const;
};