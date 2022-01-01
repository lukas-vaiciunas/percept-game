#pragma once

#include <random>
#include "AABB.h"
#include "MobileEnemy.h"
#include "TickAlarm.h"

class EnemySpawner : public AABB
{
private:
	const MobileEnemy enemyTemplate_;
	TickAlarm spawnAlarm_;

	unsigned int maxNumSpawned_;
	unsigned int numSpawned_;

	void spawn_(GridPartition<Enemy> &enemyPartition);
	void spawn_(
		GridPartition<Enemy> &enemyPartition,
		std::mt19937 &eng,
		const std::uniform_real_distribution<float> &xDis,
		const std::uniform_real_distribution<float> &yDis);
public:
	EnemySpawner(
		const AABB &bounds,
		const MobileEnemy &enemyTemplate,
		unsigned int maxNumSpawned,
		unsigned int numSpawnTicks);

	void updateOnTick(GridPartition<Enemy> &enemyPartition);

	void spawnAll(GridPartition<Enemy> &enemyPartition);

	void decrementNumSpawned();
};