#pragma once

#include <vector>

template<typename T>
class Pool
{
private:
	const unsigned int capacity_;

	std::vector<T *> objects_;
	unsigned int numLive_;
protected:
	void removeAtIndex_(unsigned int index);
	T *const object_(unsigned int index) const;
public:
	Pool(unsigned int capacity);
	virtual ~Pool();

	const typename std::vector<T *>::const_iterator cbegin() const;
	const typename std::vector<T *>::const_iterator cend() const;

	void add(T *const item);

	void clear();

	bool empty() const;
	bool full() const;
	unsigned int numLive() const;
};

template<typename T>
Pool<T>::Pool(unsigned int capacity) :
	capacity_(capacity),
	objects_(capacity, nullptr),
	numLive_(0)
{}

template<typename T>
Pool<T>::~Pool()
{
	this->clear();
}

template<typename T>
const typename std::vector<T *>::const_iterator Pool<T>::cbegin() const
{
	return objects_.cbegin();
}

template<typename T>
const typename std::vector<T *>::const_iterator Pool<T>::cend() const
{
	return objects_.cbegin() + numLive_;
}

template<typename T>
void Pool<T>::add(T *const item)
{
	objects_.at(numLive_++) = std::move(item);
}

template<typename T>
void Pool<T>::clear()
{
	for (unsigned int i = 0; i < numLive_; ++i)
		delete objects_.at(i);

	numLive_ = 0;
}

template<typename T>
bool Pool<T>::empty() const
{
	return numLive_ == 0;
}

template<typename T>
bool Pool<T>::full() const
{
	return numLive_ == capacity_;
}

template<typename T>
unsigned int Pool<T>::numLive() const
{
	return numLive_;
}

template<typename T>
void Pool<T>::removeAtIndex_(unsigned int index)
{
	--numLive_;

	delete objects_.at(index);
	objects_.at(index) = std::move(objects_.at(numLive_));
	objects_.at(numLive_) = nullptr;
}

template<typename T>
T *const Pool<T>::object_(unsigned int index) const
{
	return objects_.at(index);
}