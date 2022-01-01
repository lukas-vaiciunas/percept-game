#pragma once

#include <string>
#include "Event.h"
#include "StatMod.h"

class EventHealPlayer : public Event
{
private:
	unsigned int health_;
public:
	EventHealPlayer(unsigned int health) :
		Event(EventType::HealPlayer),
		health_(health)
	{}

	inline unsigned int health() const
	{
		return health_;
	}
};

class EventChangePlayerStats : public Event
{
private:
	StatMod::Stat stat_;
	StatMod::Operator op_;
	unsigned int amount_;
public:
	EventChangePlayerStats(StatMod::Stat stat, StatMod::Operator op, unsigned int amount) :
		Event(EventType::ChangePlayerStats),
		stat_(stat),
		op_(op),
		amount_(amount)
	{}

	inline StatMod::Stat stat() const
	{
		return stat_;
	}

	inline StatMod::Operator op() const
	{
		return op_;
	}

	inline unsigned int amount() const
	{
		return amount_;
	}
};

class EventSpawnDoor : public Event
{
private:
	float x_;
	float y_;
	const std::string levelPath_;
public:
	EventSpawnDoor(float x, float y, const std::string &levelPath) :
		Event(EventType::SpawnDoor),
		x_(x),
		y_(y),
		levelPath_(levelPath)
	{}

	inline float x() const
	{
		return x_;
	}

	inline float y() const
	{
		return y_;
	}

	inline const std::string &levelPath() const
	{
		return levelPath_;
	}
};

class EventChangeLevel : public Event
{
private:
	const std::string levelPath_;
public:
	EventChangeLevel(const std::string &levelPath) :
		Event(EventType::ChangeLevel),
		levelPath_(levelPath)
	{}

	inline const std::string &levelPath() const
	{
		return levelPath_;
	}
};

class EventPlayerDeath : public Event
{
public:
	EventPlayerDeath() :
		Event(EventType::PlayerDeath)
	{}
};