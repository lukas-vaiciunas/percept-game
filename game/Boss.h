#pragma once

class Animation;

#include <string>
#include "Enemy.h"
#include "TickAlarm.h"

class Boss : public Enemy
{
private:
	enum class Phase : unsigned char
	{
		Circle,
		Spiral,
		Target
	};

	std::string nextLevelPath_;

	Animation *animation_;

	std::pair<unsigned int, unsigned int> coords_;

	Phase phase_;

	TickAlarm phaseAlarm_;
	TickAlarm restAlarm_;
	TickAlarm safeAlarm_;
	TickAlarm healAlarm_;
	
	bool isResting_;
	bool isSafe_;

	void hurt_(
		unsigned int damage,
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition,
		EventQueue &gameEventQueue);

	void onDeath_(
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition,
		EventQueue &gameEventQueue);

	void updateBulletCollisions_(
		const BulletPool &bulletPool,
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition,
		EventQueue &gameEventQueue);

	void fireBullet_(BulletPool &bulletPool, float x, float y, float bulletSpeed, float angleRad);
public:
	Boss(
		float x, float y,
		float width, float height,
		float speed,
		unsigned int health,
		unsigned int itemLootTableId,
		unsigned int numPhaseTicks,
		unsigned int numRestTicks,
		unsigned int numSafeTicks,
		unsigned int numHealTicks,
		const std::string &nextLevelPath,
		const char *const animationPath,
		const GridPartition<Enemy> &enemyPartition);

	Boss(const Boss &other);
	Boss &operator=(const Boss &other);
	~Boss();

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