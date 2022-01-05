#pragma once

#include "LevelObjectData.h"

template<typename T>
class GenericLevelObjectData : public LevelObjectData<T>
{
public:
	GenericLevelObjectData(
		unsigned int numColsInCells,
		unsigned int numRowsInCells,
		unsigned int cellSizeInTiles);

	void updateVisibility(float cX, float cY, float viewRadius);
};

template<typename T>
GenericLevelObjectData<T>::GenericLevelObjectData(
	unsigned int numColsInCells,
	unsigned int numRowsInCells,
	unsigned int cellSizeInTiles)
	:
	LevelObjectData<T>(numColsInCells, numRowsInCells, cellSizeInTiles)
{}

template<typename T>
void GenericLevelObjectData<T>::updateVisibility(float cX, float cY, float viewRadius)
{
	const std::vector<T *> &possibleObjects = this->getPossibleObjects();

	this->clearVisibleObjectIndices_();

	for (size_t i = 0; i < possibleObjects.size(); ++i)
	{
		float distanceToObject = std::sqrtf(
			std::powf(possibleObjects.at(i)->x() + possibleObjects.at(i)->width() * 0.5f - cX, 2.0f)
			+ std::powf(possibleObjects.at(i)->y() + possibleObjects.at(i)->height() * 0.5f - cY, 2.0f));

		if (distanceToObject <= viewRadius)
			this->pushVisibleObjectIndex_(i);
	}
}