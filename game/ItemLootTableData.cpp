#include "ItemLootTableData.h"

void ItemLootTableData::emplace(unsigned int id, const ItemLootTable &itemLootTable)
{
	itemLootTables_.emplace(id, itemLootTable);
}

bool ItemLootTableData::contains(unsigned int id) const
{
	return itemLootTables_.find(id) != itemLootTables_.cend();
}

const ItemLootTable &ItemLootTableData::get(unsigned int id) const
{
	return itemLootTables_.at(id);
}