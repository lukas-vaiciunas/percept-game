#include "FontData.h"

FontData::~FontData()
{
	for (auto it = fonts_.begin(); it != fonts_.end(); ++it)
		al_destroy_font(it->second);
}

FontData &FontData::instance()
{
	static FontData fontData;
	return fontData;
}

void FontData::emplace(unsigned int size, ALLEGRO_FONT *const font)
{
	fonts_.emplace(size, font);
}

void FontData::renderText(
	const std::string &text,
	float x, float y,
	unsigned int size,
	const ALLEGRO_COLOR &color,
	bool centerVertically,
	int allegroFlags)
{
	al_draw_text(
		fonts_.at(size),
		color,
		x,
		(centerVertically ? y - al_get_font_line_height(fonts_.at(size)) * 0.5f : y),
		allegroFlags,
		text.c_str());
}