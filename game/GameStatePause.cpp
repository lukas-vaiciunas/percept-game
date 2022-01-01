#include <allegro5/allegro_primitives.h>
#include "GameStatePause.h"
#include "GameDriverState.h"
#include "GameStatePlay.h"
#include "Config.h"
#include "FontData.h"
#include "DriverEvents.h"

GameStatePause::GameStatePause() :
	GameState()
{}

void GameStatePause::updateOnKeyPress(GameDriverState &game, int allegroKeycode)
{
	switch (allegroKeycode)
	{
	case ALLEGRO_KEY_ESCAPE:
		game.changeState_(new GameStatePlay());
		break;
	case ALLEGRO_KEY_R:
		game.driverEventQueue_.send(new EventChangeDriverState(DriverStateType::Game));
		break;
	case ALLEGRO_KEY_M:
		game.driverEventQueue_.send(new EventChangeDriverState(DriverStateType::Menu));
		break;
	case ALLEGRO_KEY_Q:
		game.driverEventQueue_.send(new EventQuit());
		break;
	}
}

void GameStatePause::render(const GameDriverState &game) const
{
	ALLEGRO_TRANSFORM cameraTrans;
	ALLEGRO_TRANSFORM hudTrans;

	al_identity_transform(&cameraTrans);
	al_scale_transform(&cameraTrans, Config::gameScale, Config::gameScale);
	al_translate_transform(&cameraTrans, -game.camera_.x(), -game.camera_.y());

	al_identity_transform(&hudTrans);
	al_scale_transform(&hudTrans, Config::hudScale, Config::hudScale);

	al_use_transform(&cameraTrans);

	game.level_.render();
	game.particlePool_.render();
	game.player_.render();
	game.bulletPool_.render();

	al_use_transform(&hudTrans);

	game.healthDisplay_.render();

	al_identity_transform(&hudTrans);
	al_use_transform(&hudTrans);

	al_draw_filled_rectangle(0.0f, 0.0f, Config::displayWidth, Config::displayHeight, al_map_rgba(25, 25, 75, 125));

	FontData::instance().renderText(
		"Game Paused",
		Config::displayWidth * 0.5f,
		Config::displayHeight * 0.5f,
		Config::mediumFontSize,
		al_map_rgb(255, 255, 255),
		true,
		ALLEGRO_ALIGN_CENTER);

	FontData::instance().renderText(
		"[ESCAPE] Resume",
		Config::displayWidth * 0.5f,
		Config::displayHeight * 0.5f + Config::smallFontSize * 2.0f,
		Config::smallFontSize,
		al_map_rgb(255, 255, 255),
		true,
		ALLEGRO_ALIGN_CENTER);

	FontData::instance().renderText(
		"[R] Retry",
		Config::displayWidth * 0.5f,
		Config::displayHeight * 0.5f + Config::smallFontSize * 3.0f,
		Config::smallFontSize,
		al_map_rgb(255, 255, 255),
		true,
		ALLEGRO_ALIGN_CENTER);

	FontData::instance().renderText(
		"[M] Menu",
		Config::displayWidth * 0.5f,
		Config::displayHeight * 0.5f + Config::smallFontSize * 4.0f,
		Config::smallFontSize,
		al_map_rgb(255, 255, 255),
		true,
		ALLEGRO_ALIGN_CENTER);

	FontData::instance().renderText(
		"[Q] Quit",
		Config::displayWidth * 0.5f,
		Config::displayHeight * 0.5f + Config::smallFontSize * 5.0f,
		Config::smallFontSize,
		al_map_rgb(255, 255, 255),
		true,
		ALLEGRO_ALIGN_CENTER);
}