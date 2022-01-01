#pragma once

class EnemySpawner;

#include <vector>
#include "Enemy.h"
#include "Animation.h"

class MobileEnemy : public Enemy
{
private:
	enum class State : unsigned char
	{
		Idle,
		Chasing,
		Holding,
		Retreating
	};
	State state_;

	EnemySpawner *const spawnerSource_;

	std::vector<Animation> animations_;

	TickAlarm attackAlarm_;
	TickAlarm stunAlarm_;

	std::pair<unsigned int, unsigned int> oldCoords_;

	float velX_;
	float velY_;

	float idleRadius_;
	float chaseRadius_;
	float holdRadius_;
	float retreatRadius_;

	bool isMoving_;
	bool isStunned_;
	unsigned int animationIndex_;
	unsigned char renderFlag_;

	void hurt_(
		unsigned int damage,
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition);

	void onDeath_(
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition);

	void updateState_(const Player &player, BulletPool &bulletPool);

	void updateBulletCollisions_(
		const BulletPool &bulletPool,
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition);

	void updateTileCollisionsX_(
		const std::vector<CollidableTile *> &possibleCollidableTiles,
		float oldX,
		float levelWidth);

	void updateTileCollisionsY_(
		const std::vector<CollidableTile *> &possibleCollidableTiles,
		float oldY,
		float levelHeight);

	void updateAttacking_(BulletPool &bulletPool, float angleRadToPlayer);
	void updateOnMove_(GridPartition<Enemy> &enemyPartition);
	void changeState_(State toState);
public:
	MobileEnemy(
		float x, float y,
		float width, float height,
		float speed,
		unsigned int health,
		float idleRadius,
		float chaseRadius,
		float holdRadius,
		float retreatRadius,
		unsigned int itemLootTableId,
		const char *animationSheetPath,
		unsigned int numAttackTicks,
		unsigned int numStunTicks,
		const GridPartition<Enemy> &enemyPartition,
		EnemySpawner *const spawnerSource);

	MobileEnemy(
		const MobileEnemy &other,
		float x, float y,
		const GridPartition<Enemy> &enemyPartition,
		EnemySpawner *const spawnerSource);

	void updateOnTick(
		const Player &player,
		BulletPool &bulletPool,
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const std::vector<CollidableTile *> &possibleCollidableTiles,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition,
		EventQueue &gameEventQueue,
		float levelWidth, float levelHeight) override;

	void render() const override;
};