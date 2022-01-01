#pragma once

#include <string>
#include "GameState.h"

class GameStatePlay : public GameState
{
private:
	std::string statusString_;
	bool isThereStatus_;

	void updateStatus_(const GameDriverState &game);
public:
	GameStatePlay();

	void updateOnTick(GameDriverState &game) override;
	void updateOnMousePress(GameDriverState &game, unsigned int allegroMouseButton, int mouseX, int mouseY) override;
	void updateOnKeyPress(GameDriverState &game, int allegroKeycode) override;
	void updateOnKeyRelease(GameDriverState &game, int allegroKeycode) override;

	void render(const GameDriverState &game) const override;
};