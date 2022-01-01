#pragma once

enum class EventType : unsigned char
{
	ChangeDriverState,
	Quit,

	HealPlayer,
	ChangePlayerStats,
	SpawnDoor,
	ChangeLevel,
	PlayerDeath
};