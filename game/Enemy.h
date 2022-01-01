#pragma once

class Player;
class BulletPool;
class CollidableTile;
class ItemData;
class ItemLootTableData;
class ItemDrop;
class EventQueue;

#include "Entity.h"
#include "TickAlarm.h"
#include "GridPartition.h"

class Enemy : public Entity
{
private:
	unsigned int itemLootTableId_;
protected:
	void dropItem_(
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition) const;

	unsigned int getItemLootTableId_() const;
public:
	Enemy(
		float x, float y,
		float width, float height,
		float speed,
		unsigned int health,
		unsigned int itemLootTableId);

	virtual ~Enemy() {}

	virtual void updateOnTick(
		const Player &player,
		BulletPool &bulletPool,
		ParticlePool &particlePool,
		GridPartition<Enemy> &enemyPartition,
		const std::vector<CollidableTile *> &possibleCollidableTiles,
		const ItemData &itemData,
		const ItemLootTableData &itemLootTableData,
		GridPartition<ItemDrop> &itemDropPartition,
		EventQueue &gameEventQueue,
		float levelWidth, float levelHeight) = 0;

	virtual void render() const override = 0;
};