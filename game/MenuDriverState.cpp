#include "MenuDriverState.h"
#include "Config.h"
#include "EventQueue.h"
#include "DriverEvents.h"
#include "FontData.h"

MenuDriverState::MenuDriverState(EventQueue &driverEventQueue) :
	driverEventQueue_(driverEventQueue),
	startButton_(
		AABB(
			Config::displayWidth * 0.5f - Config::mediumFontSize * 4.0f,
			Config::displayHeight * 0.5f - Config::mediumFontSize,
			Config::mediumFontSize * 8.0f,
			Config::mediumFontSize * 2.0f),
		"Start",
		al_map_rgb(225, 225, 225),
		1),
	quitButton_(
		AABB(
			Config::displayWidth * 0.5f - Config::mediumFontSize * 4.0f,
			Config::displayHeight * 0.75f - Config::mediumFontSize,
			Config::mediumFontSize * 8.0f,
			Config::mediumFontSize * 2.0f),
		"Quit",
		al_map_rgb(225, 225, 225),
		1)
{}

void MenuDriverState::updateOnMousePress(unsigned int allegroMouseButton, int mouseX, int mouseY)
{
	if (startButton_.isPressed(allegroMouseButton, mouseX, mouseY))
	{
		driverEventQueue_.send(new EventChangeDriverState(DriverStateType::Game));
	}
	else if (quitButton_.isPressed(allegroMouseButton, mouseX, mouseY))
	{
		driverEventQueue_.send(new EventQuit());
	}
}

void MenuDriverState::render() const
{
	FontData::instance().renderText(
		"Percept",
		Config::displayWidth * 0.5f,
		Config::displayHeight * 0.25f,
		Config::largeFontSize,
		al_map_rgb(225, 225, 225),
		true,
		ALLEGRO_ALIGN_CENTER);

	startButton_.render();
	quitButton_.render();
}