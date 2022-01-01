#include "ItemData.h"
#include "Item.h"

ItemData::ItemData(const ItemData &other)
{
	for (auto it = other.items_.cbegin(); it != other.items_.cend(); ++it)
		items_.emplace(it->first, it->second->clone());
}

ItemData &ItemData::operator=(const ItemData &other)
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
		delete it->second;

	items_.clear();

	for (auto it = other.items_.cbegin(); it != other.items_.cend(); ++it)
		items_.emplace(it->first, it->second->clone());

	return *this;
}

ItemData::~ItemData()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
		delete it->second;
}

void ItemData::emplace(unsigned int id, Item *const item)
{
	items_.emplace(id, item);
}

bool ItemData::contains(unsigned int id) const
{
	return items_.find(id) != items_.cend();
}

const Item &ItemData::get(unsigned int id) const
{
	return *items_.at(id);
}