#include "GameStatePlay.h"
#include "GameDriverState.h"
#include "Config.h"
#include "Item.h"
#include "FontData.h"
#include "GameStatePause.h"

GameStatePlay::GameStatePlay() :
	GameState(),
	statusString_(""),
	isThereStatus_(false)
{}

void GameStatePlay::updateOnTick(GameDriverState &game)
{
	float visibleRadius = 64.0f;
	float cX = game.player_.x() + game.player_.width() * 0.5f;
	float cY = game.player_.y() + game.player_.height() * 0.5f;

	game.gameEventQueue_.dispatch();

	game.player_.updateOnTick(
		game.bulletPool_,
		game.particlePool_,
		game.healthDisplay_,
		game.level_.possibleCollidableTiles(),
		game.level_.visibleCollidableTileIndices(),
		game.gameEventQueue_,
		game.level_.widthInTiles() * 16.0f,
		game.level_.heightInTiles() * 16.0f);

	if (game.player_.isMoving())
	{
		game.level_.updateOnPlayerMove(game.player_, cX, cY, visibleRadius);
		game.centerCameraOnPlayer_();

		this->updateStatus_(game);

		cX = game.player_.x() + game.player_.width() * 0.5f;
		cY = game.player_.y() + game.player_.height() * 0.5f;
	}

	game.particlePool_.updateOnTick(cX, cY, visibleRadius);
	game.bulletPool_.updateOnTick(cX, cY, visibleRadius, game.level_.possibleCollidableTiles());
	game.level_.updateOnTick(game.player_, game.bulletPool_, game.particlePool_, game.gameEventQueue_, cX, cY, visibleRadius);
}

void GameStatePlay::updateOnMousePress(GameDriverState &game, unsigned int allegroMouseButton, int mouseX, int mouseY)
{
	game.player_.updateOnMousePress(
		allegroMouseButton,
		(mouseX + game.camera_.x()) / Config::gameScale,
		(mouseY + game.camera_.y()) / Config::gameScale,
		game.bulletPool_);
}

void GameStatePlay::updateOnKeyPress(GameDriverState &game, int allegroKeycode)
{
	if (allegroKeycode == ALLEGRO_KEY_F)
	{
		if (game.level_.isPlayerHoveringItemDrop())
		{
			float visibleRadius = 64.0f;
			float cX = game.player_.x() + game.player_.width() * 0.5f;
			float cY = game.player_.y() + game.player_.height() * 0.5f;

			game.level_.hoveredItem().use(game.gameEventQueue_);
			game.level_.removeHoveredItemDrop(game.player_, cX, cY, visibleRadius);
			this->updateStatus_(game);
		}
		else if (game.level_.isPlayerHoveringDoor())
		{
			game.level_.hoveredDoor().open(game.gameEventQueue_);
			this->updateStatus_(game);
		}
	}
	else if (allegroKeycode == ALLEGRO_KEY_ESCAPE)
	{
		game.changeState_(new GameStatePause());
	}
	else
	{
		game.player_.updateOnKeyPress(allegroKeycode);
	}
}

void GameStatePlay::updateOnKeyRelease(GameDriverState &game, int allegroKeycode)
{
	game.player_.updateOnKeyRelease(allegroKeycode);
}

void GameStatePlay::render(const GameDriverState &game) const
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

	if (isThereStatus_)
	{
		FontData::instance().renderText(
			statusString_,
			Config::displayWidth * 0.5f,
			Config::displayHeight * 0.75f,
			Config::mediumFontSize,
			al_map_rgb(255, 255, 255),
			true,
			ALLEGRO_ALIGN_CENTER);
	}
}

void GameStatePlay::updateStatus_(const GameDriverState &game)
{
	isThereStatus_ = false;
	if (game.level_.isPlayerHoveringItemDrop())
	{
		isThereStatus_ = true;
		statusString_ = "[F] Get Item (" + game.level_.hoveredItem().statusString() + ")";
	}
	else if (game.level_.isPlayerHoveringDoor())
	{
		isThereStatus_ = true;
		statusString_ = "[F] Enter Portal";
	}
}