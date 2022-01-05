#pragma once

class Collidable;

#include "LevelObjectData.h"

template<typename T>
class InteractableLevelObjectData : public LevelObjectData<T>
{
private:
	size_t hoveredObjectIndex_;
	bool isPlayerHoveringObject_;
public:
	InteractableLevelObjectData(
		unsigned int numColsInCells,
		unsigned int numRowsInCells,
		unsigned int cellSizeInTiles);

	void updateVisibility(const Collidable &player, float cX, float cY, float viewRadius);

	T &getHoveredObjectRef();
	const T *const getHoveredObjectPtr() const;

	const T &getHoveredObject() const;
	bool isPlayerHoveringObject() const;
};

template<typename T>
InteractableLevelObjectData<T>::InteractableLevelObjectData(
	unsigned int numColsInCells,
	unsigned int numRowsInCells,
	unsigned int cellSizeInTiles)
	:
	LevelObjectData<T>(numColsInCells, numRowsInCells, cellSizeInTiles),
	hoveredObjectIndex_(0),
	isPlayerHoveringObject_(false)
{}

template<typename T>
void InteractableLevelObjectData<T>::updateVisibility(const Collidable &player, float cX, float cY, float viewRadius)
{
	const std::vector<T *> &possibleObjects = this->getPossibleObjects();

	this->clearVisibleObjectIndices_();
	isPlayerHoveringObject_ = false;

	for (size_t i = 0; i < possibleObjects.size(); ++i)
	{
		float distanceToObject = std::sqrtf(
			std::powf(possibleObjects.at(i)->x() + possibleObjects.at(i)->width() * 0.5f - cX, 2.0f)
				+ std::powf(possibleObjects.at(i)->y() + possibleObjects.at(i)->height() * 0.5f - cY, 2.0f));

		if (distanceToObject <= viewRadius)
		{
			this->pushVisibleObjectIndex_(i);

			if (player.isCollidingWith(*possibleObjects.at(i)))
			{
				hoveredObjectIndex_ = i;
				isPlayerHoveringObject_ = true;
			}
		}
	}
}

template<typename T>
T &InteractableLevelObjectData<T>::getHoveredObjectRef()
{
	return *this->getPossibleObjects().at(hoveredObjectIndex_);
}

template<typename T>
const T *const InteractableLevelObjectData<T>::getHoveredObjectPtr() const
{
	return this->getPossibleObjects().at(hoveredObjectIndex_);
}

template<typename T>
const T &InteractableLevelObjectData<T>::getHoveredObject() const
{
	return *this->getPossibleObjects().at(hoveredObjectIndex_);
}

template<typename T>
bool InteractableLevelObjectData<T>::isPlayerHoveringObject() const
{
	return isPlayerHoveringObject_;
}