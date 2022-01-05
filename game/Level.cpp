#include <cmath>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <allegro5/bitmap_io.h>
#include "Level.h"
#include "Player.h"
#include "Boss.h"
#include "ItemDrop.h"
#include "HealthItem.h"
#include "StatItem.h"
#include "StatMod.h"

Level::Level() :
	chunks_(std::vector<std::vector<ChunkInfo>>(4, std::vector<ChunkInfo>(4, ChunkInfo(ChunkInfo::ChunkType::Empty, 0)))),
	enemySpawners_({}),
	imageData_(),
	itemData_(),
	itemLootTableData_(),
	scenicTileObjectData_(8, 8, 10),
	collidableTileObjectData_(8, 8, 10),
	enemyObjectData_(8, 8, 10),
	itemDropObjectData_(8, 8, 10),
	doorObjectData_(8, 8, 10),
	partitionCoords_({ 0, 0 }),
	chunkCoords_({ 0, 0 }),
	spawnX_(0.0f),
	spawnY_(0.0f),
	chunkWidthInTiles_(0),
	chunkHeightInTiles_(0),
	widthInTiles_(0),
	heightInTiles_(0)
{}

Level::Level(const char *const levelPath, unsigned int difficultyMult) :
	Level()
{
	this->generate_(levelPath, difficultyMult);
}

void Level::updateOnTick(
	const Player &player,
	BulletPool &bulletPool,
	ParticlePool &particlePool,
	EventQueue &gameEventQueue,
	float cX, float cY, float visibleRadius)
{
	GridPartition<Enemy> &enemyPartition = enemyObjectData_.getPartitionRef();
	GridPartition<ItemDrop> &itemDropPartition = itemDropObjectData_.getPartitionRef();

	for (ItemDrop *const itemDrop : itemDropObjectData_.getPossibleObjects())
		itemDrop->updateOnTick(itemDropPartition);

	itemDropObjectData_.updatePossibility(partitionCoords_);
	itemDropObjectData_.updateVisibility(player, cX, cY, visibleRadius);

	for (Enemy *const enemy : enemyObjectData_.getPossibleObjects())
	{
		enemy->updateOnTick(
			player,
			bulletPool,
			particlePool,
			enemyPartition,
			collidableTileObjectData_.getPossibleObjects(),
			itemData_,
			itemLootTableData_,
			itemDropPartition,
			gameEventQueue,
			widthInTiles_ * 16.0f,
			heightInTiles_ * 16.0f);

		if (!enemy->isLive())
			delete enemy;
	}

	for (EnemySpawner &enemySpawner : enemySpawners_)
		enemySpawner.updateOnTick(enemyPartition);

	enemyObjectData_.updatePossibility(partitionCoords_);
	enemyObjectData_.updateVisibility(cX, cY, visibleRadius);
}

void Level::updateOnPlayerMove(const Collidable &player, float cX, float cY, float visibleRadius)
{
	std::pair<unsigned int, unsigned int> newPartitionCoords = std::move(
		scenicTileObjectData_.posToCoords(cX, cY));

	std::pair<unsigned int, unsigned int> newChunkCoords = std::move(
		std::make_pair<unsigned int, unsigned int>(
			cX / (chunkWidthInTiles_ * 16.0f),
			cY / (chunkHeightInTiles_ * 16.0f)));

	if (newPartitionCoords != partitionCoords_)
	{
		partitionCoords_ = std::move(newPartitionCoords);
		this->updatePossibility();
	}

	if (newChunkCoords != chunkCoords_)
	{
		chunks_.at(newChunkCoords.second).at(newChunkCoords.first).discover();
		chunkCoords_ = std::move(newChunkCoords);
	}

	this->updateVisibility(player, cX, cY, visibleRadius);
}

void Level::updatePossibility()
{
	scenicTileObjectData_.updatePossibility(partitionCoords_);
	collidableTileObjectData_.updatePossibility(partitionCoords_);
	doorObjectData_.updatePossibility(partitionCoords_);
}

void Level::updateVisibility(const Collidable &player, float cX, float cY, float visibleRadius)
{
	scenicTileObjectData_.updateVisibility(cX, cY, visibleRadius);
	collidableTileObjectData_.updateVisibility(cX, cY, visibleRadius);
	doorObjectData_.updateVisibility(player, cX, cY, visibleRadius);
}

void Level::updateCoords(float cX, float cY)
{
	partitionCoords_ = std::move(scenicTileObjectData_.posToCoords(cX, cY));

	chunkCoords_ = std::move(std::make_pair<unsigned int, unsigned int>(
		cX / (chunkWidthInTiles_ * 16.0f),
		cY / (chunkHeightInTiles_ * 16.0f)));

	chunks_.at(chunkCoords_.second).at(chunkCoords_.first).discover();
}

void Level::removeHoveredItemDrop(const Collidable &player, float cX, float cY, float visibleRadius)
{
	itemDropObjectData_.getHoveredObjectRef().remove(itemDropObjectData_.getPartitionRef());
	delete itemDropObjectData_.getHoveredObjectPtr();
	itemDropObjectData_.updatePossibility(partitionCoords_);
	itemDropObjectData_.updateVisibility(player, cX, cY, visibleRadius);
}

void Level::spawnDoor(Door *const door, const Collidable &player, float cX, float cY, float visibleRadius)
{
	doorObjectData_.getPartitionRef().add(door);
	doorObjectData_.updatePossibility(partitionCoords_);
	doorObjectData_.updateVisibility(player, cX, cY, visibleRadius);
}

void Level::render() const
{
	const std::vector<ScenicTile *> &possibleScenicTiles = scenicTileObjectData_.getPossibleObjects();
	const std::vector<CollidableTile *> &possibleCollidableTiles = collidableTileObjectData_.getPossibleObjects();
	const std::vector<Enemy *> &possibleEnemies = enemyObjectData_.getPossibleObjects();
	const std::vector<ItemDrop *> &possibleItemDrops = itemDropObjectData_.getPossibleObjects();
	const std::vector<Door *> &possibleDoors = doorObjectData_.getPossibleObjects();

	for (const size_t &i : scenicTileObjectData_.getDiscoveredObjectIndices())
		possibleScenicTiles.at(i)->renderTinted(imageData_, al_map_rgb(125, 125, 175));

	for (const size_t &i : scenicTileObjectData_.getVisibleObjectIndices())
		possibleScenicTiles.at(i)->render(imageData_);

	for (const size_t &i : collidableTileObjectData_.getDiscoveredObjectIndices())
		possibleCollidableTiles.at(i)->renderTinted(imageData_, al_map_rgb(125, 125, 175));

	for (const size_t &i : collidableTileObjectData_.getVisibleObjectIndices())
		possibleCollidableTiles.at(i)->render(imageData_);

	for (const size_t &i : doorObjectData_.getDiscoveredObjectIndices())
		possibleDoors.at(i)->renderTinted(al_map_rgb(125, 125, 175));

	for (const size_t &i : doorObjectData_.getVisibleObjectIndices())
		possibleDoors.at(i)->render();
	
	for (const size_t &i : enemyObjectData_.getVisibleObjectIndices())
		possibleEnemies.at(i)->render();

	for (const size_t &i : itemDropObjectData_.getVisibleObjectIndices())
		possibleItemDrops.at(i)->render(itemData_);
}

float Level::spawnX() const
{
	return spawnX_;
}

float Level::spawnY() const
{
	return spawnY_;
}

unsigned int Level::widthInTiles() const
{
	return widthInTiles_;
}

unsigned int Level::heightInTiles() const
{
	return heightInTiles_;
}

const std::vector<std::vector<ChunkInfo>> &Level::getChunks() const
{
	return chunks_;
}

const ImageData &Level::getImageData() const
{
	return imageData_;
}

const DiscoverableLevelObjectData<CollidableTile> &Level::getCollidableTileObjectData() const
{
	return collidableTileObjectData_;
}

const InteractableLevelObjectData<ItemDrop> &Level::getItemDropObjectData() const
{
	return itemDropObjectData_;
}

const DiscoverableInteractableLevelObjectData<Door> &Level::getDoorObjectData() const
{
	return doorObjectData_;
}

const Item &Level::getHoveredItem() const
{
	return itemData_.get(itemDropObjectData_.getHoveredObject().itemId());
}

void Level::generate_(const char *const levelPath, unsigned int difficultyMult)
{
	ALLEGRO_BITMAP *sheetBitmap = al_load_bitmap("img/sheet-tiles.png");

	std::ifstream levelIn(levelPath);
	if (!levelIn.is_open())
		throw std::invalid_argument("Failed to load level data");

	std::string line = "";
	std::string token = "";
	std::istringstream ss;
	std::unordered_map<unsigned int, std::pair<std::string, unsigned int>> partMap;
	unsigned int partBossId = 0;

	std::random_device rd;
	std::mt19937 eng(rd());

	unsigned int levelWidthInChunks = 0;
	unsigned int levelHeightInChunks = 0;
	unsigned int minFillerId = 0;
	unsigned int maxFillerId = 0;

	std::getline(levelIn, line);
	ss.clear();
	ss.str(line);
	ss >> token;
	if (token == "LEVEL_WIDTH_IN_CHUNKS")
	{
		ss >> levelWidthInChunks;
	}

	std::getline(levelIn, line);
	ss.clear();
	ss.str(line);
	ss >> token;
	if (token == "LEVEL_HEIGHT_IN_CHUNKS")
	{
		ss >> levelHeightInChunks;
	}

	std::getline(levelIn, line);
	ss.clear();
	ss.str(line);
	ss >> token;
	if (token == "CHUNK_WIDTH_IN_TILES")
	{
		ss >> chunkWidthInTiles_;
	}

	std::getline(levelIn, line);
	ss.clear();
	ss.str(line);
	ss >> token;
	if (token == "CHUNK_HEIGHT_IN_TILES")
	{
		ss >> chunkHeightInTiles_;
	}

	widthInTiles_ = levelWidthInChunks * chunkWidthInTiles_;
	heightInTiles_ = levelHeightInChunks * chunkHeightInTiles_;

	std::getline(levelIn, line);
	ss.clear();
	ss.str(line);
	ss >> token;
	if (token == "FILLERS_SCENIC")
	{
		ss >> minFillerId
			>> maxFillerId;

		std::getline(levelIn, line);
		if (line == "{")
		{
			unsigned int tileId = 0;
			unsigned int sourceX = 0;
			unsigned int sourceY = 0;

			std::getline(levelIn, line);

			while (line != "}")
			{
				ss.clear();
				ss.str(line);

				ss >> tileId
					>> sourceX
					>> sourceY;

				if (!imageData_.contains(tileId))
					imageData_.emplace(tileId, Image(sheetBitmap, sourceX, sourceY, 16, 16));

				std::getline(levelIn, line);
			}
		}
	}

	std::getline(levelIn, line);
	if (line == "PARTS_RANDOM")
	{
		std::getline(levelIn, line);
		if (line == "{")
		{
			std::string partName = "";
			unsigned int partId = 0;
			unsigned int numOfPart = 0;

			std::getline(levelIn, line);

			while (line != "}")
			{
				ss.clear();
				ss.str(line);

				ss >> partId
					>> partName
					>> numOfPart;

				partMap.emplace(partId, std::make_pair("data/" + partName + ".txt", numOfPart));

				std::getline(levelIn, line);
			}
		}
	}

	std::getline(levelIn, line);
	if (line == "BOSS_RANDOM")
	{
		std::getline(levelIn, line);
		if (line == "{")
		{
			std::string partName = "";
			unsigned int partId = 0;
			unsigned int numOfPart = 0;

			std::getline(levelIn, line);

			while (line != "}")
			{
				ss.clear();
				ss.str(line);

				ss >> partId
					>> partName
					>> numOfPart;

				partMap.emplace(partId, std::make_pair("data/" + partName + ".txt", numOfPart));

				partBossId = partId;

				std::getline(levelIn, line);
			}
		}
	}

	levelIn.close();

	std::vector<std::vector<unsigned int>> chunkIds(
		levelHeightInChunks,
		std::vector<unsigned int>(
			levelWidthInChunks,
			0));

	std::uniform_int_distribution<unsigned int> chunkColDis(0, levelWidthInChunks - 1);
	std::uniform_int_distribution<unsigned int> chunkRowDis(0, levelHeightInChunks - 1);
	std::uniform_int_distribution<unsigned int> fillerIdDis(minFillerId, maxFillerId);

	GridPartition<ScenicTile> &scenicTilePartition = scenicTileObjectData_.getPartitionRef();
	GridPartition<CollidableTile> &collidableTilePartition = collidableTileObjectData_.getPartitionRef();
	GridPartition<Enemy> &enemyPartition = enemyObjectData_.getPartitionRef();

	unsigned int chunkCol = chunkColDis(eng);
	unsigned int chunkRow = chunkRowDis(eng);

	imageData_.emplace(1000, Image("img/icon-grass.png"));
	imageData_.emplace(1001, Image("img/icon-enemy.png"));
	imageData_.emplace(1002, Image("img/icon-boss.png"));

	//player spawn
	chunkIds.at(chunkRow).at(chunkCol) = 1;

	for (auto it = partMap.cbegin(); it != partMap.cend(); ++it)
	{
		for (unsigned int i = 0; i < it->second.second; ++i)
		{
			chunkCol = chunkColDis(eng);
			chunkRow = chunkRowDis(eng);

			while (chunkIds.at(chunkRow).at(chunkCol) != 0)
			{
				chunkCol = chunkColDis(eng);
				chunkRow = chunkRowDis(eng);
			}

			chunkIds.at(chunkRow).at(chunkCol) = it->first;
		}
	}

	for (unsigned int chunkRow = 0; chunkRow < levelHeightInChunks; ++chunkRow)
	{
		for (unsigned int chunkCol = 0; chunkCol < levelWidthInChunks; ++chunkCol)
		{
			unsigned int chunkId = chunkIds.at(chunkRow).at(chunkCol);
			if (chunkId == 0)
			{
				for (unsigned int tileRow = 0; tileRow < chunkHeightInTiles_; ++tileRow)
					for (unsigned int tileCol = 0; tileCol < chunkWidthInTiles_; ++tileCol)
						scenicTilePartition.add(
							new ScenicTile(
								(chunkCol * chunkWidthInTiles_ + tileCol) * 16.0f,
								(chunkRow * chunkHeightInTiles_ + tileRow) * 16.0f,
								16.0f, 16.0f,
								fillerIdDis(eng)));

				chunks_.at(chunkRow).at(chunkCol).setType(ChunkInfo::ChunkType::Empty);
				chunks_.at(chunkRow).at(chunkCol).setIconImageId(1000);
			}
			else if (chunkId == 1)
			{
				for (unsigned int tileRow = 0; tileRow < chunkHeightInTiles_; ++tileRow)
					for (unsigned int tileCol = 0; tileCol < chunkWidthInTiles_; ++tileCol)
						scenicTilePartition.add(
							new ScenicTile(
								(chunkCol * chunkWidthInTiles_ + tileCol) * 16.0f,
								(chunkRow * chunkHeightInTiles_ + tileRow) * 16.0f,
								16.0f, 16.0f,
								fillerIdDis(eng)));

				spawnX_ = (chunkCol + 0.5f) * chunkWidthInTiles_ * 16.0f - 8.0f;
				spawnY_ = (chunkRow + 0.5f) * chunkHeightInTiles_ * 16.0f - 8.0f;

				chunks_.at(chunkRow).at(chunkCol).setType(ChunkInfo::ChunkType::Empty);
				chunks_.at(chunkRow).at(chunkCol).setIconImageId(1000);
			}
			else if (chunkId == partBossId)
			{
				std::ifstream inPart(partMap.at(chunkId).first);
				if (!inPart.is_open())
					throw std::invalid_argument("Failed to load level part data");

				std::unordered_map<unsigned int, bool> collidableIds;
				unsigned int idFiller = 0;

				std::getline(inPart, line);
				ss.clear();
				ss.str(line);
				ss >> token;
				if (token == "ID_FILLER")
				{
					ss >> idFiller;
				}

				std::getline(inPart, line);
				if (line == "TILES_SCENIC")
					this->readTilesScenic_(inPart, ss, line, sheetBitmap);

				std::getline(inPart, line);
				if (line == "TILES_COLLIDABLE")
					this->readTilesCollidable_(inPart, ss, line, sheetBitmap, collidableIds);

				std::getline(inPart, line);
				if (line == "ITEMS")
					this->readItems_(inPart, ss, line);

				std::getline(inPart, line);
				if (line == "ITEM_LOOT_TABLES")
					this->readItemLootTables_(inPart, ss, line);

				std::getline(inPart, line);
				if (line == "BOSS")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int id = 0;
						float width = 0.0f;
						float height = 0.0f;
						float speed = 0.0f;
						int health = 0;
						unsigned int itemLootTableId = 0;
						unsigned int numPhaseTicks = 0;
						unsigned int numRestTicks = 0;
						unsigned int numSafeTicks = 0;
						unsigned int numHealTicks = 0;
						std::string animationPath = "";

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> id
								>> width
								>> height
								>> speed
								>> health
								>> itemLootTableId
								>> numPhaseTicks
								>> numRestTicks
								>> numSafeTicks
								>> numHealTicks
								>> animationPath;

							enemyPartition.add(
								new Boss(
									(chunkCol + 0.5f) * chunkWidthInTiles_ * 16.0f - width * 0.5f,
									(chunkRow + 0.5f) * chunkHeightInTiles_ * 16.0f - height * 0.5f,
									width, height,
									speed,
									health * difficultyMult,
									itemLootTableId,
									numPhaseTicks,
									numRestTicks,
									numSafeTicks,
									numHealTicks,
									"data/level-overworld.txt",
									animationPath.c_str(),
									enemyPartition));

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "LAYOUT")
					this->readLayout_(
						inPart,
						ss,
						line,
						fillerIdDis,
						eng,
						collidableIds,
						chunkCol,
						chunkRow,
						idFiller);

				inPart.close();

				chunks_.at(chunkRow).at(chunkCol).setType(ChunkInfo::ChunkType::Boss);
				chunks_.at(chunkRow).at(chunkCol).setIconImageId(1002);
			}
			else
			{
				std::ifstream inPart(partMap.at(chunkId).first);
				if (!inPart.is_open())
					throw std::invalid_argument("Failed to load level part data");

				std::unordered_map<unsigned int, bool> collidableIds;
				std::unordered_map<unsigned int, MobileEnemy> enemies;
				unsigned int idFiller = 0;

				std::getline(inPart, line);
				ss.clear();
				ss.str(line);
				ss >> token;
				if (token == "ID_FILLER")
				{
					ss >> idFiller;
				}

				std::getline(inPart, line);
				if (line == "TILES_SCENIC")
					this->readTilesScenic_(inPart, ss, line, sheetBitmap);

				std::getline(inPart, line);
				if (line == "TILES_COLLIDABLE")
					this->readTilesCollidable_(inPart, ss, line, sheetBitmap, collidableIds);

				std::getline(inPart, line);
				if (line == "ITEMS")
					this->readItems_(inPart, ss, line);

				std::getline(inPart, line);
				if (line == "ITEM_LOOT_TABLES")
					this->readItemLootTables_(inPart, ss, line);

				std::getline(inPart, line);
				if (line == "ENEMIES")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int id = 0;
						float speed = 0.0f;
						int health = 0;
						float idleRadius = 0.0f;
						float chaseRadius = 0.0f;
						float holdRadius = 0.0f;
						float retreatRadius = 0.0f;
						unsigned int itemLootTableId = 0;
						std::string animationSheetPath = "";
						unsigned int numAttackTicks = 0;
						unsigned int numStunTicks = 0;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> id
								>> speed
								>> health
								>> idleRadius
								>> chaseRadius
								>> holdRadius
								>> retreatRadius
								>> itemLootTableId
								>> animationSheetPath
								>> numAttackTicks
								>> numStunTicks;

							enemies.emplace(
								id,
								MobileEnemy(
									0.0f, 0.0f, 16.0f, 16.0f,
									speed,
									health * difficultyMult,
									idleRadius, chaseRadius, holdRadius, retreatRadius,
									itemLootTableId,
									animationSheetPath.c_str(),
									numAttackTicks,
									numStunTicks,
									enemyPartition,
									nullptr));

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "SPAWNERS")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int enemyId = 0;
						unsigned int relTileStartCol = 0;
						unsigned int relTileStartRow = 0;
						unsigned int relTileEndCol = 0;
						unsigned int relTileEndRow = 0;
						unsigned int maxNumSpawned = 0;
						unsigned int numSpawnTicks = 0;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> enemyId
								>> relTileStartCol
								>> relTileStartRow
								>> relTileEndCol
								>> relTileEndRow
								>> maxNumSpawned
								>> numSpawnTicks;

							enemySpawners_.push_back(
								EnemySpawner(
									AABB(
										(chunkCol * chunkWidthInTiles_ + relTileStartCol) * 16.0f,
										(chunkRow * chunkHeightInTiles_ + relTileStartRow) * 16.0f,
										(relTileEndCol - relTileStartCol) * 16.0f,
										(relTileEndRow - relTileStartRow) * 16.0f),
									enemies.at(enemyId),
									maxNumSpawned,
									numSpawnTicks));

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "LAYOUT")
					this->readLayout_(
						inPart,
						ss,
						line,
						fillerIdDis,
						eng,
						collidableIds,
						chunkCol,
						chunkRow,
						idFiller);

				inPart.close();

				chunks_.at(chunkRow).at(chunkCol).setType(ChunkInfo::ChunkType::Enemy);
				chunks_.at(chunkRow).at(chunkCol).setIconImageId(1001);
			}
		}
	}

	al_destroy_bitmap(sheetBitmap);

	for (size_t i = 0; i < enemySpawners_.size(); ++i)
		enemySpawners_.at(i).spawnAll(enemyPartition);
}

void Level::readTilesScenic_(
	std::ifstream &inFile,
	std::istringstream &iss,
	std::string &line,
	ALLEGRO_BITMAP *const sheetBitmap)
{
	std::getline(inFile, line);
	if (line == "{")
	{
		unsigned int tileId = 0;
		unsigned int sourceX = 0;
		unsigned int sourceY = 0;

		std::getline(inFile, line);
		while (line != "}")
		{
			iss.clear();
			iss.str(line);

			iss >> tileId
				>> sourceX
				>> sourceY;

			if (!imageData_.contains(tileId))
				imageData_.emplace(tileId, Image(sheetBitmap, sourceX, sourceY, 16, 16));

			std::getline(inFile, line);
		}
	}
}

void Level::readTilesCollidable_(
	std::ifstream &inFile,
	std::istringstream &iss,
	std::string &line,
	ALLEGRO_BITMAP *const sheetBitmap,
	std::unordered_map<unsigned int, bool> &collidableIds)
{
	std::getline(inFile, line);
	if (line == "{")
	{
		unsigned int tileId = 0;
		unsigned int sourceX = 0;
		unsigned int sourceY = 0;
		bool needsBackground = false;

		std::getline(inFile, line);
		while (line != "}")
		{
			iss.clear();
			iss.str(line);

			iss >> tileId
				>> sourceX
				>> sourceY
				>> needsBackground;

			if (!imageData_.contains(tileId))
				imageData_.emplace(tileId, Image(sheetBitmap, sourceX, sourceY, 16, 16));

			collidableIds.emplace(tileId, needsBackground);

			std::getline(inFile, line);
		}
	}
}

void Level::readItems_(
	std::ifstream &inFile,
	std::istringstream &iss,
	std::string &line)
{
	std::getline(inFile, line);
	if (line == "{")
	{
		unsigned int itemId = 0;
		std::string statusString = "";
		std::string imagePathStr = "";
		std::string itemTypeStr = "";

		std::getline(inFile, line);
		while (line != "}")
		{
			iss.clear();
			iss.str(line);

			iss >> itemId
				>> statusString
				>> imagePathStr
				>> itemTypeStr;

			if (itemTypeStr == "HealthItem")
			{
				unsigned int health = 0;

				iss >> health;

				if (!itemData_.contains(itemId))
					itemData_.emplace(
						itemId,
						new HealthItem(health, statusString, Image(imagePathStr.c_str())));
			}

			else if (itemTypeStr == "StatItem")
			{
				StatMod::Stat stat = StatMod::Stat::Damage;
				StatMod::Operator op = StatMod::Operator::Add;
				std::string statStr = "";
				std::string opStr = "";
				unsigned int amount = 0;

				iss >> statStr
					>> opStr
					>> amount;

				if (statStr == "Damage")
				{
					stat = StatMod::Stat::Damage;
				}
				else if (statStr == "MaxHealth")
				{
					stat = StatMod::Stat::MaxHealth;
				}

				if (opStr == "Add")
				{
					op = StatMod::Operator::Add;
				}
				else if (opStr == "Subtract")
				{
					op = StatMod::Operator::Subtract;
				}

				if (!itemData_.contains(itemId))
					itemData_.emplace(
						itemId,
						new StatItem(stat, op, amount, statusString, Image(imagePathStr.c_str())));
			}

			std::getline(inFile, line);
		}
	}
}

void Level::readItemLootTables_(
	std::ifstream &inFile,
	std::istringstream &iss,
	std::string &line)
{
	std::getline(inFile, line);
	if (line == "{")
	{
		std::vector<std::pair<unsigned int, float>> itemChances;
		unsigned int itemLootTableId = 0;
		unsigned int numItems = 0;
		unsigned int itemId = 0;
		float itemChance = 0.0f;

		std::getline(inFile, line);
		while (line != "}")
		{
			iss.clear();
			iss.str(line);

			iss >> itemLootTableId
				>> numItems;

			for (unsigned int i = 0; i < numItems; ++i)
			{
				iss >> itemId
					>> itemChance;

				itemChances.push_back({ itemId, itemChance });
			}

			if (!itemLootTableData_.contains(itemLootTableId))
				itemLootTableData_.emplace(itemLootTableId, ItemLootTable(itemChances));

			std::getline(inFile, line);
		}
	}
}

void Level::readLayout_(
	std::ifstream &inFile,
	std::istringstream &iss,
	std::string &line,
	const std::uniform_int_distribution<unsigned int> &fillerIdDis,
	std::mt19937 &eng,
	const std::unordered_map<unsigned int, bool> &collidableIds,
	unsigned int chunkCol,
	unsigned int chunkRow,
	unsigned int idFiller)
{
	std::getline(inFile, line);
	if (line == "{")
	{
		GridPartition<ScenicTile> &scenicTilePartition = scenicTileObjectData_.getPartitionRef();
		GridPartition<CollidableTile> &collidableTilePartition = collidableTileObjectData_.getPartitionRef();
		unsigned int tileId = 0;

		for (unsigned int tileRow = 0; tileRow < chunkHeightInTiles_; ++tileRow)
		{
			std::getline(inFile, line);
			iss.clear();
			iss.str(line);

			for (unsigned int tileCol = 0; tileCol < chunkWidthInTiles_; ++tileCol)
			{
				iss >> tileId;

				if (tileId == idFiller)
				{
					scenicTilePartition.add(
						new ScenicTile(
							(chunkCol * chunkWidthInTiles_ + tileCol) * 16.0f,
							(chunkRow * chunkHeightInTiles_ + tileRow) * 16.0f,
							16.0f, 16.0f,
							fillerIdDis(eng)));
				}
				else if (collidableIds.find(tileId) == collidableIds.cend())
				{
					scenicTilePartition.add(
						new ScenicTile(
							(chunkCol * chunkWidthInTiles_ + tileCol) * 16.0f,
							(chunkRow * chunkHeightInTiles_ + tileRow) * 16.0f,
							16.0f, 16.0f,
							tileId));
				}
				else
				{
					//	if a background scenic tile is needed...
					if (collidableIds.at(tileId))
						scenicTilePartition.add(
							new ScenicTile(
								(chunkCol * chunkWidthInTiles_ + tileCol) * 16.0f,
								(chunkRow * chunkHeightInTiles_ + tileRow) * 16.0f,
								16.0f, 16.0f,
								fillerIdDis(eng)));

					collidableTilePartition.add(
						new CollidableTile(
							(chunkCol * chunkWidthInTiles_ + tileCol) * 16.0f,
							(chunkRow * chunkHeightInTiles_ + tileRow) * 16.0f,
							tileId,
							imageData_));
				}
			}
		}

		std::getline(inFile, line);
	}
}