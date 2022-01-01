#pragma once

#include <unordered_map>
#include "ItemLootTable.h"

class ItemLootTableData
{
private:
	std::unordered_map<unsigned int, ItemLootTable> itemLootTables_;
public:
	ItemLootTableData() {}

	void emplace(unsigned int id, const ItemLootTable &itemLootTable);

	bool contains(unsigned int id) const;

	const ItemLootTable &get(unsigned int id) const;
};