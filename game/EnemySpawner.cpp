#include "EnemySpawner.h"

EnemySpawner::EnemySpawner(
	const AABB &bounds,
	const MobileEnemy &enemyTemplate,
	unsigned int maxNumSpawned,
	unsigned int numSpawnTicks)
	:
	AABB(bounds),
	enemyTemplate_(enemyTemplate),
	spawnAlarm_(numSpawnTicks),
	maxNumSpawned_(maxNumSpawned),
	numSpawned_(0)
{}

void EnemySpawner::updateOnTick(GridPartition<Enemy> &enemyPartition)
{
	if (numSpawned_ < maxNumSpawned_)
	{
		spawnAlarm_.updateOnTick();
		if (spawnAlarm_.isAlarm())
		{
			this->spawn_(enemyPartition);

			spawnAlarm_.reset();
		}
	}
}

void EnemySpawner::spawnAll(GridPartition<Enemy> &enemyPartition)
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<float> xDis(this->x(), this->x() + this->width() - enemyTemplate_.width());
	std::uniform_real_distribution<float> yDis(this->y(), this->y() + this->height() - enemyTemplate_.height());

	for (unsigned int i = numSpawned_; i < maxNumSpawned_; ++i)
		this->spawn_(enemyPartition, eng, xDis, yDis);
}

void EnemySpawner::decrementNumSpawned()
{
	--numSpawned_;
}

void EnemySpawner::spawn_(GridPartition<Enemy> &enemyPartition)
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<float> xDis(this->x(), this->x() + this->width() - enemyTemplate_.width());
	std::uniform_real_distribution<float> yDis(this->y(), this->y() + this->height() - enemyTemplate_.height());

	this->spawn_(enemyPartition, eng, xDis, yDis);
}

void EnemySpawner::spawn_(
	GridPartition<Enemy> &enemyPartition,
	std::mt19937 &eng,
	const std::uniform_real_distribution<float> &xDis,
	const std::uniform_real_distribution<float> &yDis)
{
	enemyPartition.add(new MobileEnemy(enemyTemplate_, xDis(eng), yDis(eng), enemyPartition, this));

	++numSpawned_;
}