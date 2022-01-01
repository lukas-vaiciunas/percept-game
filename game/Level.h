#pragma once

#include <vector>
#include "ScenicTile.h"
#include "CollidableTile.h"
#include "EnemySpawner.h"
#include "Enemy.h"
#include "GridPartition.h"
#include "ImageData.h"
#include "ItemDrop.h"
#include "ItemData.h"
#include "ItemLootTableData.h"
#include "Door.h"

class Level
{
private:
	ImageData imageData_;
	ItemData itemData_;
	ItemLootTableData itemLootTableData_;

	GridPartition<ScenicTile> scenicTilesPartition_;
	std::vector<ScenicTile *> possibleScenicTiles_;
	std::vector<size_t> visibleScenicTileIndices_;
	std::vector<size_t> discoveredScenicTileIndices_;

	GridPartition<CollidableTile> collidableTilesPartition_;
	std::vector<CollidableTile *> possibleCollidableTiles_;
	std::vector<size_t> visibleCollidableTileIndices_;
	std::vector<size_t> discoveredCollidableTileIndices_;

	std::vector<EnemySpawner> enemySpawners_;
	GridPartition<Enemy> enemyPartition_;
	std::vector<Enemy *> possibleEnemies_;
	std::vector<size_t> visibleEnemyIndices_;

	GridPartition<ItemDrop> itemDropPartition_;
	std::vector<ItemDrop *> possibleItemDrops_;
	std::vector<size_t> visibleItemDropIndices_;

	GridPartition<Door> doorPartition_;
	std::vector<Door *> possibleDoors_;
	std::vector<size_t> visibleDoorIndices_;
	std::vector<size_t> discoveredDoorIndices_;

	std::pair<unsigned int, unsigned int> oldCoords_;

	float spawnX_;
	float spawnY_;

	unsigned int widthInTiles_;
	unsigned int heightInTiles_;

	bool isPlayerHoveringItemDrop_;
	unsigned int hoveredItemDropIndex_;

	bool isPlayerHoveringDoor_;
	unsigned int hoveredDoorIndex_;

	void updateItemDropsVisibility_(const Player &player, float cX, float cY, float visibleRadius);
	void updateDoorsPossibility_();
	void updateDoorsVisibility_(const Player &player, float cX, float cY, float visibleRadius);
public:
	Level();
	Level(const char *const levelPath, unsigned int difficultyMult);

	void generate(const char *const levelPath, unsigned int difficultyMult);

	void updateOnTick(
		const Player &player,
		BulletPool &bulletPool,
		ParticlePool &particlePool,
		EventQueue &gameEventQueue,
		float cX, float cY, float visibleRadius);

	void updateOnPlayerMove(const Player &player, float cX, float cY, float visibleRadius);
	void updatePossibility();
	void updateVisibility(const Player &player, float cX, float cY, float visibleRadius);
	void updateCoords(float cX, float cY);

	void removeHoveredItemDrop(const Player &player, float cX, float cY, float visibleRadius);

	void spawnDoor(Door *const door, const Player &player, float cX, float cY, float visibleRadius);

	void render() const;

	float spawnX() const;
	float spawnY() const;

	unsigned int widthInTiles() const;
	unsigned int heightInTiles() const;

	const std::vector<CollidableTile *> &possibleCollidableTiles() const;
	const std::vector<size_t> &visibleCollidableTileIndices() const;
	bool isPlayerHoveringItemDrop() const;
	const Item &hoveredItem() const;
	bool isPlayerHoveringDoor() const;
	const Door &hoveredDoor() const;
};