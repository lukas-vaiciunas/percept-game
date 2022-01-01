#pragma once

class Item;

#include <unordered_map>

class ItemData
{
private:
	std::unordered_map<unsigned int, Item *> items_;
public:
	ItemData() {}
	ItemData(const ItemData &other);
	ItemData &operator=(const ItemData &other);
	~ItemData();

	void emplace(unsigned int id, Item *const item);

	bool contains(unsigned int id) const;
	const Item &get(unsigned int id) const;
};