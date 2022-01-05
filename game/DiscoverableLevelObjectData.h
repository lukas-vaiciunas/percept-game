#pragma once

#include "LevelObjectData.h"

template<typename T>
class DiscoverableLevelObjectData : public LevelObjectData<T>
{
private:
	std::vector<size_t> discoveredObjectIndices_;
public:
	DiscoverableLevelObjectData(
		unsigned int numColsInCells,
		unsigned int numRowsInCells,
		unsigned int cellSizeInTiles);

	void updateVisibility(float cX, float cY, float viewRadius);

	const std::vector<size_t> &getDiscoveredObjectIndices() const;
};

template<typename T>
DiscoverableLevelObjectData<T>::DiscoverableLevelObjectData(
	unsigned int numColsInCells,
	unsigned int numRowsInCells,
	unsigned int cellSizeInTiles)
	:
	LevelObjectData<T>(numColsInCells, numRowsInCells, cellSizeInTiles),
	discoveredObjectIndices_({})
{}

template<typename T>
void DiscoverableLevelObjectData<T>::updateVisibility(float cX, float cY, float viewRadius)
{
	const std::vector<T *> &possibleObjects = this->getPossibleObjects();

	this->clearVisibleObjectIndices_();
	discoveredObjectIndices_.clear();

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
		}
		else if (object->isDiscovered())
		{
			discoveredObjectIndices_.push_back(i);
		}
	}
}

template<typename T>
const std::vector<size_t> &DiscoverableLevelObjectData<T>::getDiscoveredObjectIndices() const
{
	return discoveredObjectIndices_;
}