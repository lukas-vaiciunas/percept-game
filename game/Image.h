#pragma once

#include <allegro5/bitmap.h>
#include <allegro5/allegro_color.h>

class Image
{
private:
	ALLEGRO_BITMAP *bitmap_;
	unsigned char width_;
	unsigned char height_;

	void copy_(const Image &other);
	void destroy_();
public:
	Image(const char *path);
	Image(const ALLEGRO_COLOR &color, unsigned char width, unsigned char height);
	Image(const char *sheetPath, unsigned int sX, unsigned int sY, unsigned char sWidth, unsigned char sHeight);
	Image(ALLEGRO_BITMAP *const sheetBitmap, unsigned int sX, unsigned int sY, unsigned char sWidth, unsigned char sHeight);
	Image(const Image &other);
	Image &operator=(const Image &other);
	~Image();

	void render(float x, float y, int allegroFlags = 0) const;
	void renderScaled(float x, float y, float scale, int allegroFlags = 0) const;
	void renderRotated(float x, float y, float angleRad, bool centered, int allegroFlags = 0) const;
	void renderScaledRotated(float x, float y, float scale, float angleRad, bool centered, int allegroFlags = 0) const;
	void renderTinted(float x, float y, const ALLEGRO_COLOR &tint, int allegroFlags = 0) const;

	unsigned char width() const;
	unsigned char height() const;
};