#include "ImageData.h"

ImageData::ImageData() :
	images_({})
{}

void ImageData::emplace(unsigned int id, const Image &image)
{
	images_.emplace(id, image);
}

bool ImageData::contains(unsigned int id) const
{
	return images_.find(id) != images_.cend();
}

const Image &ImageData::getImage(unsigned int id) const
{
	return images_.at(id);
}