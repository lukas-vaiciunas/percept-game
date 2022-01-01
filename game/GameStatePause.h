#pragma once

#include "GameState.h"

class GameStatePause : public GameState
{
public:
	GameStatePause();

	void updateOnKeyPress(GameDriverState &game, int allegroKeycode) override;

	void render(const GameDriverState &game) const override;
};