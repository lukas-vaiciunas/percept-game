#include <random>
#include "ItemLootTable.h"

ItemLootTable::ItemLootTable(const std::vector<std::pair<unsigned int, float>> &itemChances) :
	itemChances_(itemChances)
{}

unsigned int ItemLootTable::roll() const
{
	const float dice = 1000000.0f;

	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<float> rollDis(0, dice);

	float roll = rollDis(eng);

	for (auto it = itemChances_.cbegin(); it != itemChances_.cend(); ++it)
	{
		float chance = dice * it->second;

		if (roll <= chance)
		{
			return it->first;
		}
		else
		{
			roll -= chance;
		}
	}

	return itemChances_.back().first;
}