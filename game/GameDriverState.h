#pragma once

class GameState;

#include "DriverState.h"
#include "Listener.h"
#include "EventQueue.h"
#include "Player.h"
#include "Enemy.h"
#include "Level.h"
#include "Camera.h"
#include "BulletPool.h"
#include "ParticlePool.h"
#include "HealthDisplay.h"

class GameDriverState :
	public DriverState,
	public Listener
{
private:
	friend class GameStatePlay;
	friend class GameStateLoss;
	friend class GameStatePause;

	GameState *state_;

	EventQueue &driverEventQueue_;

	EventQueue gameEventQueue_;

	Player player_;
	Level level_;
	Camera camera_;
	BulletPool bulletPool_;
	ParticlePool particlePool_;

	HealthDisplay healthDisplay_;

	unsigned int difficultyMult_;

	void centerCameraOnPlayer_();
	void changeLevel_(const char *const levelPath);
	void changeState_(GameState *newState);
public:
	GameDriverState(EventQueue &driverEventQueue);
	~GameDriverState();

	void onEvent(const Event &ev) override;

	void updateOnTick() override;
	void updateOnMousePress(unsigned int allegroMouseButton, int mouseX, int mouseY) override;
	void updateOnKeyPress(int allegroKeycode) override;
	void updateOnKeyRelease(int allegroKeycode) override;

	void render() const override;
};