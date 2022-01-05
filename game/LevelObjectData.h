#pragma once

#include <cmath>
#include "GridPartition.h"

template<typename T>
class LevelObjectData
{
private:
	GridPartition<T> partition_;
	std::vector<T *> possibleObjects_;
	std::vector<size_t> visibleObjectIndices_;
protected:
	void clearVisibleObjectIndices_();
	void pushVisibleObjectIndex_(size_t index);
public:
	LevelObjectData(
		unsigned int numColsInCells,
		unsigned int numRowsInCells,
		unsigned int cellSizeInTiles);

	virtual ~LevelObjectData() {}

	void updatePossibility(const std::pair<unsigned int, unsigned int> &coords);

	std::pair<unsigned int, unsigned int> posToCoords(float x, float y) const;

	GridPartition<T> &getPartitionRef();

	const GridPartition<T> &getPartition() const;
	const std::vector<T *> &getPossibleObjects() const;
	const std::vector<size_t> &getVisibleObjectIndices() const;
};

template<typename T>
LevelObjectData<T>::LevelObjectData(
	unsigned int numColsInCells,
	unsigned int numRowsInCells,
	unsigned int cellSizeInTiles)
	:
	partition_(numColsInCells, numRowsInCells, cellSizeInTiles),
	possibleObjects_({}),
	visibleObjectIndices_({})
{}

template<typename T>
void LevelObjectData<T>::updatePossibility(const std::pair<unsigned int, unsigned int> &coords)
{
	possibleObjects_ = std::move(partition_.getItemsNearCoords(coords.first, coords.second, 1, 1));
}

template<typename T>
std::pair<unsigned int, unsigned int> LevelObjectData<T>::posToCoords(float x, float y) const
{
	return partition_.posToCoords(x, y);
}

template<typename T>
GridPartition<T> &LevelObjectData<T>::getPartitionRef()
{
	return partition_;
}

template<typename T>
const GridPartition<T> &LevelObjectData<T>::getPartition() const
{
	return partition_;
}

template<typename T>
const std::vector<T *> &LevelObjectData<T>::getPossibleObjects() const
{
	return possibleObjects_;
}

template<typename T>
const std::vector<size_t> &LevelObjectData<T>::getVisibleObjectIndices() const
{
	return visibleObjectIndices_;
}

template<typename T>
void LevelObjectData<T>::clearVisibleObjectIndices_()
{
	visibleObjectIndices_.clear();
}

template<typename T>
void LevelObjectData<T>::pushVisibleObjectIndex_(size_t index)
{
	visibleObjectIndices_.push_back(index);
}