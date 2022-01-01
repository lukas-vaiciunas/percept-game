#pragma once

#include <unordered_map>
#include "Image.h"

class ImageData
{
private:
	std::unordered_map<unsigned int, Image> images_;
public:
	ImageData();

	void emplace(unsigned int id, const Image &image);

	bool contains(unsigned int id) const;
	const Image &getImage(unsigned int id) const;
};