#include "Enemy.h"
#include "Image.h"
#include "ItemData.h"
#include "ItemLootTableData.h"
#include "Item.h"
#include "ItemDrop.h"

Enemy::Enemy(
	float x, float y,
	float width, float height,
	float speed,
	unsigned int health,
	unsigned int itemLootTableId)
	:
	Entity(x, y, width, height, speed, health, 1),
	itemLootTableId_(itemLootTableId)
{}

void Enemy::dropItem_(
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition) const
{
	unsigned int itemId = itemLootTableData.get(this->getItemLootTableId_()).roll();

	if (itemId != 0)
	{
		const Image &itemImage = itemData.get(itemId).image();
		float itemHeight = itemImage.height();

		itemDropPartition.add(
			new ItemDrop(
				this->x() + this->width() * 0.5f - itemImage.width() * 0.5f,
				this->y() + this->height() * 0.5f - itemHeight * 0.5f,
				this->y() + this->height() - itemHeight * 0.5f,
				0.0f, -2.0f,
				0.15f,
				itemId,
				itemData,
				itemDropPartition));
	}
}

unsigned int Enemy::getItemLootTableId_() const
{
	return itemLootTableId_;
}