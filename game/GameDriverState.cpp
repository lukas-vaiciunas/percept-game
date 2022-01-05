#include "GameDriverState.h"
#include "Config.h"
#include "GameEvents.h"
#include "Item.h"
#include "GameStatePlay.h"
#include "GameStateLoss.h"

GameDriverState::GameDriverState(EventQueue &driverEventQueue) :
	DriverState(),
	Listener(
		{
			EventType::HealPlayer,
			EventType::ChangePlayerStats,
			EventType::SpawnDoor,
			EventType::ChangeLevel,
			EventType::PlayerDeath
		}),
	driverEventQueue_(driverEventQueue),
	gameEventQueue_(),
	player_(0.0f, 0.0f, 16.0f, 16.0f, 1.5f, 6, "img/player-sheet.png", 60),
	level_(),
	camera_(0.0f, 0.0f),
	bulletPool_(100),
	particlePool_(100),
	healthDisplay_(1.0f, 1.0f),
	minimapDisplay_(
		Config::displayWidth / Config::hudScale - 34.0f - 1.0f,
		1.0f,
		4, 4),
	difficultyMult_(1)
{
	gameEventQueue_.attach(this);

	this->changeState_(new GameStatePlay());
	this->changeLevel_("data/level-overworld.txt");

	healthDisplay_.update(player_.health(), player_.maxHealth());
}

GameDriverState::~GameDriverState()
{
	delete state_;
}

void GameDriverState::onEvent(const Event &ev)
{
	if (ev.type() == EventType::HealPlayer)
	{
		const EventHealPlayer &trueEv = static_cast<const EventHealPlayer &>(ev);

		player_.heal(trueEv.health());
		healthDisplay_.update(player_.health(), player_.maxHealth());
	}
	else if (ev.type() == EventType::ChangePlayerStats)
	{
		const EventChangePlayerStats &trueEv = static_cast<const EventChangePlayerStats &>(ev);
	
		switch (trueEv.stat())
		{
		case StatMod::Stat::MaxHealth:
			if (trueEv.op() == StatMod::Operator::Add)
			{
				player_.addMaxHealth(trueEv.amount());
				player_.heal(trueEv.amount());
			}
			else if (trueEv.op() == StatMod::Operator::Subtract)
			{
				if (player_.maxHealth() <= trueEv.amount())
				{
					player_.setMaxHealth(2);
				}
				else
				{
					player_.subMaxHealth(trueEv.amount());
				}
			}

			healthDisplay_.update(player_.health(), player_.maxHealth());
			break;
		case StatMod::Stat::Damage:
			if (trueEv.op() == StatMod::Operator::Add)
			{
				player_.addDamage(trueEv.amount());
			}
			else if (trueEv.op() == StatMod::Operator::Subtract)
			{
				if (player_.damage() <= trueEv.amount())
				{
					player_.setDamage(1);
				}
				else
				{
					player_.subDamage(trueEv.amount());
				}
			}
			break;
		}
	}
	else if (ev.type() == EventType::SpawnDoor)
	{
		const EventSpawnDoor &trueEv = static_cast<const EventSpawnDoor &>(ev);

		level_.spawnDoor(
			new Door(trueEv.x(), trueEv.y(), trueEv.levelPath(), Image("img/portal-overworld.png")),
			player_,
			player_.x() + player_.width() * 0.5f,
			player_.y() + player_.height() * 0.5f,
			64.0f);
	}
	else if (ev.type() == EventType::ChangeLevel)
	{
		const EventChangeLevel &trueEv = static_cast<const EventChangeLevel &>(ev);

		++difficultyMult_;
		this->changeLevel_(trueEv.levelPath().c_str());
	}
	else if (ev.type() == EventType::PlayerDeath)
	{
		this->changeState_(new GameStateLoss());
	}
}

void GameDriverState::updateOnTick()
{
	state_->updateOnTick(*this);
}

void GameDriverState::updateOnMousePress(unsigned int allegroMouseButton, int mouseX, int mouseY)
{
	state_->updateOnMousePress(*this, allegroMouseButton, mouseX, mouseY);
}

void GameDriverState::updateOnKeyPress(int allegroKeycode)
{
	state_->updateOnKeyPress(*this, allegroKeycode);
}

void GameDriverState::updateOnKeyRelease(int allegroKeycode)
{
	state_->updateOnKeyRelease(*this, allegroKeycode);
}

void GameDriverState::render() const
{
	state_->render(*this);
}

void GameDriverState::centerCameraOnPlayer_()
{
	camera_.centerOn(player_.x() + player_.width() * 0.5f, player_.y() + player_.height() * 0.5f);
	camera_.constrain(AABB(0.0f, 0.0f, level_.widthInTiles() * 16.0f, level_.heightInTiles() * 16.0f));
}

void GameDriverState::changeLevel_(const char *const levelPath)
{
	level_ = Level(levelPath, difficultyMult_);
	player_.spawn(level_.spawnX(), level_.spawnY());
	minimapDisplay_.updateOnPlayerMove(player_.x(), player_.y());

	float playerCX = player_.x() + player_.width() * 0.5f;
	float playerCY = player_.y() + player_.height() * 0.5f;

	level_.updateCoords(playerCX, playerCY);
	level_.updatePossibility();
	level_.updateVisibility(player_, playerCX, playerCY, 64.0f);

	this->centerCameraOnPlayer_();
}

void GameDriverState::changeState_(GameState *newState)
{
	delete state_;

	state_ = newState;
}