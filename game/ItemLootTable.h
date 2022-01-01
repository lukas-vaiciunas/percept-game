#pragma once

#include <vector>

class ItemLootTable
{
private:
	std::vector<std::pair<unsigned int, float>> itemChances_;
public:
	ItemLootTable(const std::vector<std::pair<unsigned int, float>> &itemChances);

	unsigned int roll() const;
};