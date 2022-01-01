#pragma once

class GameDriverState;

class GameState
{
public:
	GameState() {}
	virtual ~GameState() {}

	virtual void updateOnTick(GameDriverState &game) {}
	virtual void updateOnMousePress(GameDriverState &game, unsigned int allegroMouseButton, int mouseX, int mouseY) {}
	virtual void updateOnKeyPress(GameDriverState &game, int allegroKeycode) {}
	virtual void updateOnKeyRelease(GameDriverState &game, int allegroKeycode) {}

	virtual void render(const GameDriverState &game) const {}
};