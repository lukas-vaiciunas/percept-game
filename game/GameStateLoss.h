#pragma once

#include "GameState.h"

class GameStateLoss : public GameState
{
public:
	GameStateLoss();

	void updateOnTick(GameDriverState &game) override;
	void updateOnKeyPress(GameDriverState &game, int allegroKeycode) override;

	void render(const GameDriverState &game) const override;
};