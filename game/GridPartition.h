#pragma once

#include "GridPartitionCell.h"

template<typename T>
class GridPartition
{
private:
	std::vector<std::vector<GridPartitionCell<T>>> cells_;
	unsigned int numCols_;
	unsigned int numRows_;
	unsigned int cellSizeInPixels_;

	unsigned int spatialHashCoords_(unsigned int col, unsigned int row);
	unsigned int spatialHashPos_(float x, float y);
public:
	GridPartition(unsigned int numCols, unsigned int numRows, unsigned int cellSizeInTiles);

	void add(T *const item);
	void addAtCoords(const std::pair<unsigned int, unsigned int> &coords, T *const item);
	void addAtCoords(unsigned int col, unsigned int row, T *const item);

	void remove(const T *const item);
	void removeAtCoords(const std::pair<unsigned int, unsigned int> &coords, const T *const item);
	void removeAtCoords(unsigned int col, unsigned int row, const T *const item);

	const std::vector<T *> getItemsNearPos(
		float x, float y,
		unsigned int radiusXInCells, unsigned int radiusYInCells) const;

	const std::vector<T *> getItemsNearCoords(
		unsigned int col, unsigned int row,
		unsigned int radiusXInCells, unsigned int radiusYInCells) const;

	const std::pair<unsigned int, unsigned int> posToCoords(float x, float y) const;
};

template<typename T>
GridPartition<T>::GridPartition(unsigned int numCols, unsigned int numRows, unsigned int cellSizeInTiles) :
	cells_(numRows, std::vector<GridPartitionCell<T>>(numCols, GridPartitionCell<T>())),
	numCols_(numCols),
	numRows_(numRows),
	cellSizeInPixels_(cellSizeInTiles * 16)
{}

template<typename T>
void GridPartition<T>::add(T *const item)
{
	std::pair<unsigned int, unsigned int> coords = this->posToCoords(item->x() + item->width() * 0.5f, item->y() + item->height() * 0.5f);

	this->addAtCoords(coords, item);
}

template<typename T>
void GridPartition<T>::addAtCoords(const std::pair<unsigned int, unsigned int> &coords, T *const item)
{
	cells_.at(coords.second).at(coords.first).add(item);
}

template<typename T>
void GridPartition<T>::addAtCoords(unsigned int col, unsigned int row, T *const item)
{
	cells_.at(row).at(col).add(item);
}

template<typename T>
void GridPartition<T>::remove(const T *const item)
{
	std::pair<unsigned int, unsigned int> coords = this->posToCoords(item->x() + item->width() * 0.5f, item->y() + item->height() * 0.5f);

	this->removeAtCoords(coords, item);
}

template<typename T>
void GridPartition<T>::removeAtCoords(const std::pair<unsigned int, unsigned int> &coords, const T *const item)
{
	cells_.at(coords.second).at(coords.first).remove(item);
}

template<typename T>
void GridPartition<T>::removeAtCoords(unsigned int col, unsigned int row, const T *const item)
{
	cells_.at(row).at(col).remove(item);
}

template<typename T>
const std::vector<T *> GridPartition<T>::getItemsNearPos(
	float x, float y,
	unsigned int radiusXInCells, unsigned int radiusYInCells) const
{
	std::pair<unsigned int, unsigned int> coords = this->posToCoords(x, y);

	return this->getTilesNearCoords(coords.first, coords.second, radiusXInCells, radiusYInCells);
}

template<typename T>
const std::vector<T *> GridPartition<T>::getItemsNearCoords(
	unsigned int col, unsigned int row,
	unsigned int radiusXInCells, unsigned int radiusYInCells) const
{
	unsigned int minCol = (col > radiusXInCells ? col - radiusXInCells : 0);
	unsigned int minRow = (row > radiusYInCells ? row - radiusYInCells : 0);

	unsigned int maxCol = (col < numCols_ - radiusXInCells - 1 ? col + radiusXInCells : numCols_ - 1);
	unsigned int maxRow = (row < numRows_ - radiusYInCells - 1 ? row + radiusYInCells : numRows_ - 1);

	std::vector<T *> result = {};

	for (unsigned int inRow = minRow; inRow <= maxRow; ++inRow)
	{
		for (unsigned int inCol = minCol; inCol <= maxCol; ++inCol)
		{
			const std::vector<T *> &targets = cells_.at(inRow).at(inCol).getItems();

			result.insert(result.cend(), targets.cbegin(), targets.cend());
		}
	}

	return result;
}

template<typename T>
const std::pair<unsigned int, unsigned int> GridPartition<T>::posToCoords(float x, float y) const
{
	return std::make_pair<unsigned int, unsigned int>(
		static_cast<unsigned int>(x / cellSizeInPixels_),
		static_cast<unsigned int>(y / cellSizeInPixels_));
}

template<typename T>
unsigned int GridPartition<T>::spatialHashCoords_(unsigned int col, unsigned int row)
{
	return col + row * numCols_;
}

template<typename T>
unsigned int GridPartition<T>::spatialHashPos_(float x, float y)
{
	std::pair<unsigned int, unsigned int> coords = this->posToCoords(x, y);

	return this->spatialHashCoords_(coords.first, coords.second);
}