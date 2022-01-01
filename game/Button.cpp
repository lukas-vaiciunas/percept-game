#include <allegro5/allegro_primitives.h>
#include "Button.h"
#include "FontData.h"
#include "Config.h"

Button::Button(
	const AABB &bounds,
	const std::string &text,
	const ALLEGRO_COLOR &color,
	unsigned int allegroMouseButton)
	:
	Collidable(bounds),
	text_(text),
	color_(color),
	allegroMouseButton_(allegroMouseButton)
{}

bool Button::isPressed(unsigned int allegroMouseButton, int mouseX, int mouseY)
{
	return allegroMouseButton == allegroMouseButton_
		&& this->contains(mouseX, mouseY);
}

void Button::render() const
{
	al_draw_filled_rectangle(
		this->x(),
		this->y(),
		this->x() + this->width(),
		this->y() + this->height(),
		color_);

	FontData::instance().renderText(
		text_,
		this->x() + this->width() * 0.5f,
		this->y() + this->height() * 0.5f,
		Config::mediumFontSize,
		al_map_rgb(25, 25, 75),
		true,
		ALLEGRO_ALIGN_CENTER);
}