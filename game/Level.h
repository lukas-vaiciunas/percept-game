#pragma once

#include <vector>
#include "ChunkInfo.h"
#include "ScenicTile.h"
#include "CollidableTile.h"
#include "EnemySpawner.h"
#include "Enemy.h"
#include "ImageData.h"
#include "ItemDrop.h"
#include "ItemData.h"
#include "ItemLootTableData.h"
#include "Door.h"
#include "GenericLevelObjectData.h"
#include "DiscoverableLevelObjectData.h"
#include "InteractableLevelObjectData.h"
#include "DiscoverableInteractableLevelObjectData.h"

class Level
{
private:
	std::vector<std::vector<ChunkInfo>> chunks_;

	std::vector<EnemySpawner> enemySpawners_;

	ImageData imageData_;
	ItemData itemData_;
	ItemLootTableData itemLootTableData_;

	DiscoverableLevelObjectData<ScenicTile> scenicTileObjectData_;
	DiscoverableLevelObjectData<CollidableTile> collidableTileObjectData_;
	GenericLevelObjectData<Enemy> enemyObjectData_;
	InteractableLevelObjectData<ItemDrop> itemDropObjectData_;
	DiscoverableInteractableLevelObjectData<Door> doorObjectData_;

	std::pair<unsigned int, unsigned int> partitionCoords_;
	std::pair<unsigned int, unsigned int> chunkCoords_;

	float spawnX_;
	float spawnY_;

	unsigned int chunkWidthInTiles_;
	unsigned int chunkHeightInTiles_;

	unsigned int widthInTiles_;
	unsigned int heightInTiles_;

	void generate_(const char *const levelPath, unsigned int difficultyMult);

	void readTilesScenic_(
		std::ifstream &inFile,
		std::istringstream &iss,
		std::string &line,
		ALLEGRO_BITMAP *const sheetBitmap);

	void readTilesCollidable_(
		std::ifstream &inFile,
		std::istringstream &iss,
		std::string &line,
		ALLEGRO_BITMAP *const sheetBitmap,
		std::unordered_map<unsigned int, bool> &collidableIds);

	void readItems_(
		std::ifstream &inFile,
		std::istringstream &iss,
		std::string &line);

	void readItemLootTables_(
		std::ifstream &inFile,
		std::istringstream &iss,
		std::string &line);

	void readLayout_(
		std::ifstream &inFile,
		std::istringstream &iss,
		std::string &line,
		const std::uniform_int_distribution<unsigned int> &fillerIdDis,
		std::mt19937 &eng,
		const std::unordered_map<unsigned int, bool> &collidableIds,
		unsigned int chunkCol,
		unsigned int chunkRow,
		unsigned int idFiller);
public:
	Level();
	Level(const char *const levelPath, unsigned int difficultyMult);

	void updateOnTick(
		const Player &player,
		BulletPool &bulletPool,
		ParticlePool &particlePool,
		EventQueue &gameEventQueue,
		float cX, float cY, float visibleRadius);

	void updateOnPlayerMove(const Collidable &player, float cX, float cY, float visibleRadius);
	void updatePossibility();
	void updateVisibility(const Collidable &player, float cX, float cY, float visibleRadius);
	void updateCoords(float cX, float cY);

	void removeHoveredItemDrop(const Collidable &player, float cX, float cY, float visibleRadius);

	void spawnDoor(Door *const door, const Collidable &player, float cX, float cY, float visibleRadius);

	void render() const;

	float spawnX() const;
	float spawnY() const;

	unsigned int widthInTiles() const;
	unsigned int heightInTiles() const;

	const std::vector<std::vector<ChunkInfo>> &getChunks() const;
	const ImageData &getImageData() const;
	const DiscoverableLevelObjectData<CollidableTile> &getCollidableTileObjectData() const;
	const InteractableLevelObjectData<ItemDrop> &getItemDropObjectData() const;
	const DiscoverableInteractableLevelObjectData<Door> &getDoorObjectData() const;
	const Item &getHoveredItem() const;
};