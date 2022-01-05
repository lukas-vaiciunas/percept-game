#pragma once

class ItemData;

#include "Collidable.h"
#include "Cloneable.h"
#include "GridPartition.h"

class ItemDrop :
	public Collidable,
	public Cloneable
{
private:
	std::pair<unsigned int, unsigned int> coords_;

	unsigned int itemId_;

	float maxY_;

	float velX_;
	float velY_;
	float accY_;

	bool isMoving_;

	void updateOnMove_(GridPartition<ItemDrop> &itemDropPartition);
public:
	ItemDrop(
		float x, float y, float maxY,
		float velX, float velY,
		float accY,
		unsigned int itemId,
		const ItemData &itemData,
		const GridPartition<ItemDrop> &itemDropPartition);

	ItemDrop *clone() const override;

	void updateOnTick(GridPartition<ItemDrop> &itemDropPartition);

	void render(const ItemData &itemData) const;

	void remove(GridPartition<ItemDrop> &itemDropPartition);

	unsigned int itemId() const;
};