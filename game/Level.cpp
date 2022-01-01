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
	imageData_(),
	itemData_(),
	itemLootTableData_(),
	
	scenicTilesPartition_(8, 8, 10),
	possibleScenicTiles_({}),
	visibleScenicTileIndices_({}),
	discoveredScenicTileIndices_({}),
	
	collidableTilesPartition_(8, 8, 10),
	possibleCollidableTiles_({}),
	visibleCollidableTileIndices_({}),
	discoveredCollidableTileIndices_({}),

	enemySpawners_({}),
	enemyPartition_(8, 8, 10),
	possibleEnemies_({}),
	visibleEnemyIndices_({}),

	itemDropPartition_(8, 8, 10),
	possibleItemDrops_({}),
	visibleItemDropIndices_({}),

	doorPartition_(8, 8, 10),
	possibleDoors_({}),
	visibleDoorIndices_({}),
	discoveredDoorIndices_({}),
	
	oldCoords_({ 0, 0 }),
	spawnX_(0.0f),
	spawnY_(0.0f),
	widthInTiles_(0),
	heightInTiles_(0),
	isPlayerHoveringItemDrop_(false),
	hoveredItemDropIndex_(0),
	isPlayerHoveringDoor_(false),
	hoveredDoorIndex_(false)
{}

Level::Level(const char *const levelPath, unsigned int difficultyMult) :
	Level()
{
	this->generate(levelPath, difficultyMult);
}

void Level::generate(const char *const levelPath, unsigned int difficultyMult)
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
	unsigned int chunkWidthInTiles = 0;
	unsigned int chunkHeightInTiles = 0;
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
		ss >> chunkWidthInTiles;
	}

	std::getline(levelIn, line);
	ss.clear();
	ss.str(line);
	ss >> token;
	if (token == "CHUNK_HEIGHT_IN_TILES")
	{
		ss >> chunkHeightInTiles;
	}

	widthInTiles_ = levelWidthInChunks * chunkWidthInTiles;
	heightInTiles_ = levelHeightInChunks * chunkHeightInTiles;

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

	unsigned int chunkCol = chunkColDis(eng);
	unsigned int chunkRow = chunkRowDis(eng);

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
				for (unsigned int tileRow = 0; tileRow < chunkHeightInTiles; ++tileRow)
					for (unsigned int tileCol = 0; tileCol < chunkWidthInTiles; ++tileCol)
						scenicTilesPartition_.add(
							new ScenicTile(
								(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
								(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
								16.0f, 16.0f,
								fillerIdDis(eng)));
			}
			else if (chunkId == 1)
			{
				for (unsigned int tileRow = 0; tileRow < chunkHeightInTiles; ++tileRow)
					for (unsigned int tileCol = 0; tileCol < chunkWidthInTiles; ++tileCol)
						scenicTilesPartition_.add(
							new ScenicTile(
								(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
								(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
								16.0f, 16.0f,
								fillerIdDis(eng)));

				spawnX_ = (chunkCol + 0.5f) * chunkWidthInTiles * 16.0f - 8.0f;
				spawnY_ = (chunkRow + 0.5f) * chunkHeightInTiles * 16.0f - 8.0f;
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
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int tileId = 0;
						unsigned int sourceX = 0;
						unsigned int sourceY = 0;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> tileId
								>> sourceX
								>> sourceY;

							if (!imageData_.contains(tileId))
								imageData_.emplace(tileId, Image(sheetBitmap, sourceX, sourceY, 16, 16));

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "TILES_COLLIDABLE")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int tileId = 0;
						unsigned int sourceX = 0;
						unsigned int sourceY = 0;
						bool needsBackground = false;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> tileId
								>> sourceX
								>> sourceY
								>> needsBackground;

							if (!imageData_.contains(tileId))
								imageData_.emplace(tileId, Image(sheetBitmap, sourceX, sourceY, 16, 16));
							
							collidableIds.emplace(tileId, needsBackground);

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "ITEMS")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int itemId = 0;
						std::string statusString = "";
						std::string imagePathStr = "";
						std::string itemTypeStr = "";

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> itemId
								>> statusString
								>> imagePathStr
								>> itemTypeStr;

							if (itemTypeStr == "HealthItem")
							{
								unsigned int health = 0;

								ss >> health;

								if(!itemData_.contains(itemId))
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

								ss >> statStr
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

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "ITEM_LOOT_TABLES")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						std::vector<std::pair<unsigned int, float>> itemChances;
						unsigned int itemLootTableId = 0;
						unsigned int numItems = 0;
						unsigned int itemId = 0;
						float itemChance = 0.0f;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> itemLootTableId
								>> numItems;

							for (unsigned int i = 0; i < numItems; ++i)
							{
								ss >> itemId
									>> itemChance;

								itemChances.push_back({ itemId, itemChance });
							}

							if (!itemLootTableData_.contains(itemLootTableId))
								itemLootTableData_.emplace(itemLootTableId, ItemLootTable(itemChances));

							std::getline(inPart, line);
						}
					}
				}

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

							enemyPartition_.add(
								new Boss(
									(chunkCol + 0.5f) *chunkWidthInTiles * 16.0f - width * 0.5f,
									(chunkRow + 0.5f) *chunkWidthInTiles * 16.0f - height * 0.5f,
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
									enemyPartition_));

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "LAYOUT")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int tileId = 0;

						for (unsigned int tileRow = 0; tileRow < chunkHeightInTiles; ++tileRow)
						{
							std::getline(inPart, line);
							ss.clear();
							ss.str(line);

							for (unsigned int tileCol = 0; tileCol < chunkWidthInTiles; ++tileCol)
							{
								ss >> tileId;

								if (tileId == idFiller)
								{
									scenicTilesPartition_.add(
										new ScenicTile(
											(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
											(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
											16.0f, 16.0f,
											fillerIdDis(eng)));
								}
								else if (collidableIds.find(tileId) == collidableIds.cend())
								{
									scenicTilesPartition_.add(
										new ScenicTile(
											(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
											(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
											16.0f, 16.0f,
											tileId));
								}
								else
								{
									//	if a background scenic tile is needed...
									if (collidableIds.at(tileId))
										scenicTilesPartition_.add(
											new ScenicTile(
												(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
												(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
												16.0f, 16.0f,
												fillerIdDis(eng)));

									collidableTilesPartition_.add(
										new CollidableTile(
											(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
											(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
											tileId,
											imageData_));
								}
							}
						}

						std::getline(inPart, line);
					}
				}

				inPart.close();
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
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int tileId = 0;
						unsigned int sourceX = 0;
						unsigned int sourceY = 0;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> tileId
								>> sourceX
								>> sourceY;

							if (!imageData_.contains(tileId))
								imageData_.emplace(tileId, Image(sheetBitmap, sourceX, sourceY, 16, 16));

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "TILES_COLLIDABLE")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int tileId = 0;
						unsigned int sourceX = 0;
						unsigned int sourceY = 0;
						bool needsBackground = false;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> tileId
								>> sourceX
								>> sourceY
								>> needsBackground;

							if(!imageData_.contains(tileId))
								imageData_.emplace(tileId, Image(sheetBitmap, sourceX, sourceY, 16, 16));
							
							collidableIds.emplace(tileId, needsBackground);

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "ITEMS")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int itemId = 0;
						std::string statusString = "";
						std::string imagePathStr = "";
						std::string itemTypeStr = "";

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> itemId
								>> statusString
								>> imagePathStr
								>> itemTypeStr;

							if (itemTypeStr == "HealthItem")
							{
								unsigned int health = 0;

								ss >> health;

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

								ss >> statStr
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

							std::getline(inPart, line);
						}
					}
				}

				std::getline(inPart, line);
				if (line == "ITEM_LOOT_TABLES")
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						std::vector<std::pair<unsigned int, float>> itemChances;
						unsigned int itemLootTableId = 0;
						unsigned int numItems = 0;
						unsigned int itemId = 0;
						float itemChance = 0.0f;

						std::getline(inPart, line);
						while (line != "}")
						{
							ss.clear();
							ss.str(line);

							ss >> itemLootTableId
								>> numItems;

							for (unsigned int i = 0; i < numItems; ++i)
							{
								ss >> itemId
									>> itemChance;

								itemChances.push_back({ itemId, itemChance });
							}

							if (!itemLootTableData_.contains(itemLootTableId))
								itemLootTableData_.emplace(itemLootTableId, ItemLootTable(itemChances));

							std::getline(inPart, line);
						}
					}
				}

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
									enemyPartition_,
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
										(chunkCol *chunkWidthInTiles + relTileStartCol) * 16.0f,
										(chunkRow *chunkHeightInTiles + relTileStartRow) * 16.0f,
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
				{
					std::getline(inPart, line);
					if (line == "{")
					{
						unsigned int tileId = 0;

						for (unsigned int tileRow = 0; tileRow < chunkHeightInTiles; ++tileRow)
						{
							std::getline(inPart, line);
							ss.clear();
							ss.str(line);

							for (unsigned int tileCol = 0; tileCol < chunkWidthInTiles; ++tileCol)
							{
								ss >> tileId;

								if (tileId == idFiller)
								{
									scenicTilesPartition_.add(
										new ScenicTile(
											(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
											(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
											16.0f, 16.0f,
											fillerIdDis(eng)));
								}
								else if (collidableIds.find(tileId) == collidableIds.cend())
								{
									scenicTilesPartition_.add(
										new ScenicTile(
											(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
											(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
											16.0f, 16.0f,
											tileId));
								}
								else
								{
									//	if a background scenic tile is needed...
									if(collidableIds.at(tileId))
										scenicTilesPartition_.add(
											new ScenicTile(
												(chunkCol * chunkWidthInTiles + tileCol) * 16.0f,
												(chunkRow * chunkHeightInTiles + tileRow) * 16.0f,
												16.0f, 16.0f,
												fillerIdDis(eng)));

									collidableTilesPartition_.add(
										new CollidableTile(
											(chunkCol *chunkWidthInTiles + tileCol) * 16.0f,
											(chunkRow *chunkHeightInTiles + tileRow) * 16.0f,
											tileId,
											imageData_));
								}
							}
						}
							
						std::getline(inPart, line);
					}
				}

				inPart.close();
			}
		}
	}

	al_destroy_bitmap(sheetBitmap);

	for (size_t i = 0; i < enemySpawners_.size(); ++i)
		enemySpawners_.at(i).spawnAll(enemyPartition_);
}

void Level::updateOnTick(
	const Player &player,
	BulletPool &bulletPool,
	ParticlePool &particlePool,
	EventQueue &gameEventQueue,
	float cX, float cY, float visibleRadius)
{
	for (ItemDrop *const itemDrop : possibleItemDrops_)
		itemDrop->updateOnTick(itemDropPartition_);

	this->updateItemDropsVisibility_(player, cX, cY, visibleRadius);

	for (Enemy *const enemy : possibleEnemies_)
	{
		enemy->updateOnTick(
			player,
			bulletPool,
			particlePool,
			enemyPartition_,
			possibleCollidableTiles_,
			itemData_,
			itemLootTableData_,
			itemDropPartition_,
			gameEventQueue,
			widthInTiles_ * 16.0f,
			heightInTiles_ * 16.0f);

		if (!enemy->isLive())
			delete enemy;
	}

	for (EnemySpawner &enemySpawner : enemySpawners_)
		enemySpawner.updateOnTick(enemyPartition_);

	possibleEnemies_ = enemyPartition_.getItemsNearCoords(oldCoords_.first, oldCoords_.second, 1, 1);
	visibleEnemyIndices_.clear();
	for (size_t i = 0; i < possibleEnemies_.size(); ++i)
	{
		float distanceToEnemy = std::sqrtf(
			std::powf(
				possibleEnemies_.at(i)->x() + possibleEnemies_.at(i)->width() * 0.5f - cX, 2.0f)
					+ std::powf(possibleEnemies_.at(i)->y() + possibleEnemies_.at(i)->height() * 0.5f - cY, 2.0f));

		if (distanceToEnemy <= visibleRadius)
			visibleEnemyIndices_.push_back(i);
	}
}

void Level::updateOnPlayerMove(const Player &player, float cX, float cY, float visibleRadius)
{
	const std::pair<unsigned int, unsigned int> newCoords = scenicTilesPartition_.posToCoords(cX, cY);

	if (newCoords != oldCoords_)
	{
		oldCoords_ = newCoords;
		this->updatePossibility();
	}

	this->updateVisibility(player, cX, cY, visibleRadius);
}

void Level::updatePossibility()
{
	possibleScenicTiles_ = scenicTilesPartition_.getItemsNearCoords(oldCoords_.first, oldCoords_.second, 1, 1);
	possibleCollidableTiles_ = collidableTilesPartition_.getItemsNearCoords(oldCoords_.first, oldCoords_.second, 1, 1);
	this->updateDoorsPossibility_();
}

void Level::updateVisibility(const Player &player, float cX, float cY, float visibleRadius)
{
	visibleScenicTileIndices_.clear();
	discoveredScenicTileIndices_.clear();

	for (size_t i = 0; i < possibleScenicTiles_.size(); ++i)
	{
		float distanceToTile = std::sqrtf(
			std::powf(
				possibleScenicTiles_.at(i)->x() + 8.0f - cX, 2.0f)
					+ std::powf(possibleScenicTiles_.at(i)->y() + 8.0f - cY, 2.0f));

		if (distanceToTile <= visibleRadius)
		{
			possibleScenicTiles_.at(i)->discover();
			visibleScenicTileIndices_.push_back(i);
		}
		else if (possibleScenicTiles_.at(i)->isDiscovered())
		{
			discoveredScenicTileIndices_.push_back(i);
		}
	}

	visibleCollidableTileIndices_.clear();
	discoveredCollidableTileIndices_.clear();

	for (size_t i = 0; i < possibleCollidableTiles_.size(); ++i)
	{
		float distanceToTile = std::sqrtf(
			std::powf(possibleCollidableTiles_.at(i)->x() + 8.0f - cX, 2.0f)
				+ std::powf(possibleCollidableTiles_.at(i)->y() + 8.0f - cY, 2.0f));

		if (distanceToTile <= visibleRadius)
		{
			possibleCollidableTiles_.at(i)->discover();
			visibleCollidableTileIndices_.push_back(i);
		}
		else if (possibleCollidableTiles_.at(i)->isDiscovered())
		{
			discoveredCollidableTileIndices_.push_back(i);
		}
	}

	this->updateDoorsVisibility_(player, cX, cY, visibleRadius);
}

void Level::updateCoords(float cX, float cY)
{
	oldCoords_ = scenicTilesPartition_.posToCoords(cX, cY);
}

void Level::removeHoveredItemDrop(const Player &player, float cX, float cY, float visibleRadius)
{
	possibleItemDrops_.at(hoveredItemDropIndex_)->remove(itemDropPartition_);
	delete possibleItemDrops_.at(hoveredItemDropIndex_);
	this->updateItemDropsVisibility_(player, cX, cY, visibleRadius);
}

void Level::spawnDoor(Door *const door, const Player &player, float cX, float cY, float visibleRadius)
{
	doorPartition_.add(door);
	this->updateDoorsPossibility_();
	this->updateDoorsVisibility_(player, cX, cY, visibleRadius);
}

void Level::render() const
{
	for (const unsigned int &i : discoveredScenicTileIndices_)
		possibleScenicTiles_.at(i)->renderTinted(imageData_, al_map_rgb(125, 125, 175));

	for (const unsigned int &i : visibleScenicTileIndices_)
		possibleScenicTiles_.at(i)->render(imageData_);

	for (const unsigned int &i : discoveredCollidableTileIndices_)
		possibleCollidableTiles_.at(i)->renderTinted(imageData_, al_map_rgb(125, 125, 175));

	for (const unsigned int &i : visibleCollidableTileIndices_)
		possibleCollidableTiles_.at(i)->render(imageData_);

	for(const unsigned int &i : discoveredDoorIndices_)
		possibleDoors_.at(i)->renderTinted(al_map_rgb(125, 125, 175));

	for (const unsigned int &i : visibleDoorIndices_)
		possibleDoors_.at(i)->render();

	for (const unsigned int &i : visibleEnemyIndices_)
		possibleEnemies_.at(i)->render();

	for (const unsigned int &i : visibleItemDropIndices_)
		possibleItemDrops_.at(i)->render(itemData_);
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

const std::vector<CollidableTile *> &Level::possibleCollidableTiles() const
{
	return possibleCollidableTiles_;
}

const std::vector<size_t> &Level::visibleCollidableTileIndices() const
{
	return visibleCollidableTileIndices_;
}

bool Level::isPlayerHoveringItemDrop() const
{
	return isPlayerHoveringItemDrop_;
}

const Item &Level::hoveredItem() const
{
	return itemData_.get(possibleItemDrops_.at(hoveredItemDropIndex_)->itemId());
}

bool Level::isPlayerHoveringDoor() const
{
	return isPlayerHoveringDoor_;
}

const Door &Level::hoveredDoor() const
{
	return *possibleDoors_.at(hoveredDoorIndex_);
}

void Level::updateItemDropsVisibility_(const Player &player, float cX, float cY, float visibleRadius)
{
	possibleItemDrops_ = itemDropPartition_.getItemsNearCoords(oldCoords_.first, oldCoords_.second, 1, 1);
	visibleItemDropIndices_.clear();
	isPlayerHoveringItemDrop_ = false;
	for (size_t i = 0; i < possibleItemDrops_.size(); ++i)
	{
		float distanceToItemDrop = std::sqrtf(
			std::powf(possibleItemDrops_.at(i)->x() + possibleItemDrops_.at(i)->width() * 0.5f - cX, 2.0f)
				+ std::powf(possibleItemDrops_.at(i)->y() + possibleItemDrops_.at(i)->height() * 0.5f - cY, 2.0f));

		if (distanceToItemDrop <= visibleRadius)
		{
			visibleItemDropIndices_.push_back(i);

			if (player.isCollidingWith(*possibleItemDrops_.at(i)))
			{
				isPlayerHoveringItemDrop_ = true;
				hoveredItemDropIndex_ = i;
			}
		}
	}
}

void Level::updateDoorsPossibility_()
{
	possibleDoors_ = doorPartition_.getItemsNearCoords(oldCoords_.first, oldCoords_.second, 1, 1);
}

void Level::updateDoorsVisibility_(const Player &player, float cX, float cY, float visibleRadius)
{
	visibleDoorIndices_.clear();
	discoveredDoorIndices_.clear();
	isPlayerHoveringDoor_ = false;

	for (size_t i = 0; i < possibleDoors_.size(); ++i)
	{
		float distanceToDoor = std::sqrtf(
			std::powf(possibleDoors_.at(i)->x() + possibleDoors_.at(i)->width() * 0.5f - cX, 2.0f)
			+ std::powf(possibleDoors_.at(i)->y() + possibleDoors_.at(i)->height() * 0.5f - cY, 2.0f));

		if (distanceToDoor <= visibleRadius)
		{
			visibleDoorIndices_.push_back(i);
			possibleDoors_.at(i)->discover();

			if (player.isCollidingWith(*possibleDoors_.at(i)))
			{
				isPlayerHoveringDoor_ = true;
				hoveredDoorIndex_ = i;
			}
		}
		else if(possibleDoors_.at(i)->isDiscovered())
		{
			discoveredDoorIndices_.push_back(i);
		}
	}
}