#pragma once

#include <vector>

template<typename T>
class GridPartitionCell
{
private:
	std::vector<T *> items_;
public:
	GridPartitionCell() {}
	~GridPartitionCell();

	void add(T *const item);
	void remove(const T *const item);

	const std::vector<T *> &getItems() const;
};

template<typename T>
GridPartitionCell<T>::~GridPartitionCell()
{
	for (size_t i = 0; i < items_.size(); ++i)
		delete items_.at(i);
}

template<typename T>
void GridPartitionCell<T>::add(T *const item)
{
	items_.push_back(item);
}

template<typename T>
void GridPartitionCell<T>::remove(const T *const item)
{
	for (unsigned int i = 0; i < items_.size(); ++i)
	{
		if (items_.at(i) == item)
		{
			items_.at(i) = std::move(items_.back());
			items_.pop_back();
			return;
		}
	}
}

template<typename T>
const std::vector<T *> &GridPartitionCell<T>::getItems() const
{
	return items_;
}