#pragma once

class Collidable;

#include "LevelObjectData.h"

template<typename T>
class DiscoverableInteractableLevelObjectData : public LevelObjectData<T>
{
private:
	std::vector<size_t> discoveredObjectIndices_;
	size_t hoveredObjectIndex_;
	bool isPlayerHoveringObject_;
public:
	DiscoverableInteractableLevelObjectData(
		unsigned int numColsInCells,
		unsigned int numRowsInCells,
		unsigned int cellSizeInTiles);

	void updateVisibility(const Collidable &player, float cX, float cY, float viewRadius);

	T &getHoveredObjectRef();
	const T *const getHoveredObjectPtr() const;

	const std::vector<size_t> &getDiscoveredObjectIndices() const;
	const T &getHoveredObject() const;
	bool isPlayerHoveringObject() const;
};

template<typename T>
DiscoverableInteractableLevelObjectData<T>::DiscoverableInteractableLevelObjectData(
	unsigned int numColsInCells,
	unsigned int numRowsInCells,
	unsigned int cellSizeInTiles)
	:
	LevelObjectData<T>(numColsInCells, numRowsInCells, cellSizeInTiles),
	discoveredObjectIndices_({}),
	hoveredObjectIndex_(0),
	isPlayerHoveringObject_(false)
{}

template<typename T>
void DiscoverableInteractableLevelObjectData<T>::updateVisibility(const Collidable &player, float cX, float cY, float viewRadius)
{
	const std::vector<T *> &possibleObjects = this->getPossibleObjects();

	this->clearVisibleObjectIndices_();
	discoveredObjectIndices_.clear();
	isPlayerHoveringObject_ = false;

	for (size_t i = 0; i < possibleObjects.size(); ++i)
	{
		T *const object = possibleObjects.at(i);

		float distanceToObject = std::sqrtf(
			std::powf(object->x() + object->width() * 0.5f - cX, 2.0f)
				+ std::powf(object->y() + object->height() * 0.5f - cY, 2.0f));

		if (distanceToObject <= viewRadius)
		{
			object->discover();
			this->pushVisibleObjectIndex_(i);

			if (player.isCollidingWith(*possibleObjects.at(i)))
			{
				hoveredObjectIndex_ = i;
				isPlayerHoveringObject_ = true;
			}
		}
		else if (object->isDiscovered())
		{
			discoveredObjectIndices_.push_back(i);
		}
	}
}

template<typename T>
T &DiscoverableInteractableLevelObjectData<T>::getHoveredObjectRef()
{
	return *this->getPossibleObjects().at(hoveredObjectIndex_);
}

template<typename T>
const T *const DiscoverableInteractableLevelObjectData<T>::getHoveredObjectPtr() const
{
	return this->getPossibleObjects().at(hoveredObjectIndex_);
}

template<typename T>
const std::vector<size_t> &DiscoverableInteractableLevelObjectData<T>::getDiscoveredObjectIndices() const
{
	return discoveredObjectIndices_;
}

template<typename T>
const T &DiscoverableInteractableLevelObjectData<T>::getHoveredObject() const
{
	return *this->getPossibleObjects().at(hoveredObjectIndex_);
}

template<typename T>
bool DiscoverableInteractableLevelObjectData<T>::isPlayerHoveringObject() const
{
	return isPlayerHoveringObject_;
}