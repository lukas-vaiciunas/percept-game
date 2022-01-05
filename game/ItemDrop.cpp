#include "ItemDrop.h"
#include "ItemData.h"
#include "Item.h"

ItemDrop::ItemDrop(
	float x, float y, float maxY,
	float velX, float velY,
	float accY,
	unsigned int itemId,
	const ItemData &itemData,
	const GridPartition<ItemDrop> &itemDropPartition)
	:
	Collidable(x, y, itemData.get(itemId).image().width(), itemData.get(itemId).image().height()),
	coords_(
		std::move(
			itemDropPartition.posToCoords(
				this->x() + this->width() * 0.5f,
				this->y() + this->height() * 0.5f))),
	itemId_(itemId),
	maxY_(maxY),
	velX_(velX),
	velY_(velY),
	accY_(accY),
	isMoving_(true)
{}

ItemDrop *ItemDrop::clone() const
{
	return new ItemDrop(*this);
}

void ItemDrop::updateOnTick(GridPartition<ItemDrop> &itemDropPartition)
{
	if (isMoving_)
	{
		this->moveX_(velX_);
		this->moveY_(velY_);

		this->updateOnMove_(itemDropPartition);

		velY_ += accY_;

		if (this->y() >= maxY_)
		{
			this->setY_(maxY_);
			velX_ = 0.0f;
			velY_ = 0.0f;
			isMoving_ = false;
		}
	}
}

void ItemDrop::render(const ItemData &itemData) const
{
	itemData
		.get(itemId_)
		.image()
		.render(this->x(), this->y());
}

void ItemDrop::remove(GridPartition<ItemDrop> &itemDropPartition)
{
	itemDropPartition.removeAtCoords(coords_, this);
}

unsigned int ItemDrop::itemId() const
{
	return itemId_;
}

void ItemDrop::updateOnMove_(GridPartition<ItemDrop> &itemDropPartition)
{
	std::pair<unsigned int, unsigned int> newCoords = std::move(
		itemDropPartition.posToCoords(
			this->x() + this->width() * 0.5f,
			this->y() + this->height() * 0.5f));

	if (coords_ != newCoords)
	{
		itemDropPartition.removeAtCoords(coords_, this);
		itemDropPartition.addAtCoords(newCoords, this);

		coords_ = std::move(newCoords);
	}
}