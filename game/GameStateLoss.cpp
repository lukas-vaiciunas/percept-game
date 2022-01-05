#include <allegro5/allegro_primitives.h>
#include "GameStateLoss.h"
#include "GameDriverState.h"
#include "Config.h"
#include "Item.h"
#include "DriverEvents.h"
#include "FontData.h"

GameStateLoss::GameStateLoss() :
	GameState()
{}

void GameStateLoss::updateOnTick(GameDriverState &game)
{
	float visibleRadius = 64.0f;
	float cX = game.player_.x() + game.player_.width() * 0.5f;
	float cY = game.player_.y() + game.player_.height() * 0.5f;

	game.gameEventQueue_.dispatch();

	game.particlePool_.updateOnTick(cX, cY, visibleRadius);
	game.bulletPool_.updateOnTick(cX, cY, visibleRadius, game.level_.getCollidableTileObjectData().getPossibleObjects());
	game.level_.updateOnTick(game.player_, game.bulletPool_, game.particlePool_, game.gameEventQueue_, cX, cY, visibleRadius);
}

void GameStateLoss::updateOnKeyPress(GameDriverState &game, int allegroKeycode)
{
	if (allegroKeycode == ALLEGRO_KEY_ESCAPE)
	{
		game.driverEventQueue_.send(new EventChangeDriverState(DriverStateType::Menu));
	}
	else if (allegroKeycode == ALLEGRO_KEY_R)
	{
		game.driverEventQueue_.send(new EventChangeDriverState(DriverStateType::Game));
	}
}

void GameStateLoss::render(const GameDriverState &game) const
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
	game.bulletPool_.render();

	al_use_transform(&hudTrans);

	game.healthDisplay_.render();

	al_identity_transform(&hudTrans);
	al_use_transform(&hudTrans);

	al_draw_filled_rectangle(0.0f, 0.0f, Config::displayWidth, Config::displayHeight, al_map_rgba(25, 25, 75, 125));
	
	FontData::instance().renderText(
		"You Died!",
		Config::displayWidth * 0.5f,
		Config::displayHeight * 0.5f,
		Config::mediumFontSize, 
		al_map_rgb(255, 255, 255),
		true,
		ALLEGRO_ALIGN_CENTER);

	FontData::instance().renderText(
		"[ESCAPE] Main Menu",
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
}