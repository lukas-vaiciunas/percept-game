#include <allegro5/bitmap_draw.h>
#include <allegro5/allegro_opengl.h>
#include "Image.h"

Image::Image(const char *path)
{
	bitmap_ = al_load_bitmap(path);
	width_ = al_get_bitmap_width(bitmap_);
	height_ = al_get_bitmap_height(bitmap_);

	al_convert_mask_to_alpha(bitmap_, al_map_rgb(255, 0, 255));
}

Image::Image(const ALLEGRO_COLOR &color, unsigned char width, unsigned char height)
{
	bitmap_ = al_create_bitmap(width, height);
	width_ = width;
	height_ = height;

	al_set_target_bitmap(bitmap_);
	al_clear_to_color(color);
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
}

Image::Image(const char *sheetPath, unsigned int sX, unsigned int sY, unsigned char sWidth, unsigned char sHeight)
{
	ALLEGRO_BITMAP *sheetBitmap = al_load_bitmap(sheetPath);

	bitmap_ = al_create_bitmap(sWidth, sHeight);
	width_ = sWidth;
	height_ = sHeight;

	al_set_target_bitmap(bitmap_);
	al_draw_bitmap_region(sheetBitmap, sX, sY, sWidth, sHeight, 0.0f, 0.0f, 0);
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));

	al_convert_mask_to_alpha(bitmap_, al_map_rgb(255, 0, 255));

	al_destroy_bitmap(sheetBitmap);
}

Image::Image(ALLEGRO_BITMAP *const sheetBitmap, unsigned int sX, unsigned int sY, unsigned char sWidth, unsigned char sHeight)
{
	bitmap_ = al_create_bitmap(sWidth, sHeight);
	width_ = sWidth;
	height_ = sHeight;

	al_set_target_bitmap(bitmap_);
	al_draw_bitmap_region(sheetBitmap, sX, sY, sWidth, sHeight, 0.0f, 0.0f, 0);
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));

	al_convert_mask_to_alpha(bitmap_, al_map_rgb(255, 0, 255));
}

Image::Image(const Image &other)
{
	this->copy_(other);
}

Image &Image::operator=(const Image &other)
{
	this->destroy_();
	this->copy_(other);

	return *this;
}

Image::~Image()
{
	this->destroy_();
}

void Image::render(float x, float y, int allegroFlags) const
{
	al_draw_bitmap(bitmap_, x, y, allegroFlags);
}

void Image::renderScaled(float x, float y, float scale, int allegroFlags) const
{
	al_draw_scaled_bitmap(bitmap_, 0.0f, 0.0f, width_, height_, x, y, width_ * scale, height_ * scale, allegroFlags);
}

void Image::renderRotated(float x, float y, float angleRad, bool centered, int allegroFlags) const
{
	float relCX = (centered ? width_ * 0.5f : 0.0f);
	float relCY = (centered ? height_ * 0.5f : 0.0f);

	al_draw_rotated_bitmap(bitmap_, relCX, relCY, x + relCX, y + relCY, angleRad, allegroFlags);
}

void Image::renderScaledRotated(float x, float y, float scale, float angleRad, bool centered, int allegroFlags) const
{
	float relCX = (centered ? width_ * 0.5f : 0.0f);
	float relCY = (centered ? height_ * 0.5f : 0.0f);

	al_draw_scaled_rotated_bitmap(bitmap_, relCX, relCY, x + relCX, y + relCY, scale, scale, angleRad, allegroFlags);
}

void Image::renderTinted(float x, float y, const ALLEGRO_COLOR &tint, int allegroFlags) const
{
	al_draw_tinted_bitmap(bitmap_, tint, x, y, allegroFlags);
}

unsigned char Image::width() const
{
	return width_;
}

unsigned char Image::height() const
{
	return height_;
}

void Image::copy_(const Image &other)
{
	bitmap_ = al_create_bitmap(other.width_, other.height_);
	width_ = other.width_;
	height_ = other.height_;

	al_set_target_bitmap(bitmap_);
	al_draw_bitmap(other.bitmap_, 0.0f, 0.0f, 0);
	al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
}

void Image::destroy_()
{
	al_destroy_bitmap(bitmap_);
	width_ = 0;
	height_ = 0;
}