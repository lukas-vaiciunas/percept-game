#pragma once

#include <allegro5/allegro_color.h>
#include <string>
#include "Collidable.h"

class Button : public Collidable
{
private:
	const std::string text_;
	const ALLEGRO_COLOR color_;
	unsigned int allegroMouseButton_;
public:
	Button(
		const AABB &bounds,
		const std::string &text,
		const ALLEGRO_COLOR &color,
		unsigned int allegroMouseButton);

	bool isPressed(unsigned int allegroMouseButton, int mouseX, int mouseY);

	void render() const;
};