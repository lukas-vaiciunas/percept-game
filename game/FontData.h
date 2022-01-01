#pragma once

#include <allegro5/allegro_font.h>
#include <unordered_map>

class FontData
{
private:
	std::unordered_map<unsigned int, ALLEGRO_FONT *> fonts_;

	FontData() {}
	~FontData();
public:
	FontData(const FontData &) = delete;
	FontData &operator=(const FontData &) = delete;

	static FontData &instance();

	void emplace(unsigned int size, ALLEGRO_FONT *const font);

	void renderText(
		const std::string &text,
		float x, float y,
		unsigned int size,
		const ALLEGRO_COLOR &color,
		bool centerVertically = false,
		int allegroFlags = 0);
};