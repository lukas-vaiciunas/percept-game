#include "Camera.h"
#include "Config.h"
#include "AABB.h"

Camera::Camera(float x, float y) :
	x_(x),
	y_(y)
{}

void Camera::centerOn(float x, float y)
{
	x_ = x * Config::gameScale - Config::displayWidth * 0.5f;
	y_ = y * Config::gameScale - Config::displayHeight * 0.5f;
}

void Camera::constrain(const AABB &bounds)
{
	float minX = bounds.x() * Config::gameScale;
	float minY = bounds.y() * Config::gameScale;
	float maxX = minX + bounds.width() * Config::gameScale;
	float maxY = minY + bounds.height() * Config::gameScale;

	if (x_ < minX) x_ = minX;
	else if (x_ > maxX - Config::displayWidth) x_ = maxX - Config::displayWidth;
	
	if (y_ < minY) y_ = minY;
	else if (y_ > maxY - Config::displayHeight) y_ = maxY - Config::displayHeight;
}

float Camera::x() const
{
	return x_;
}

float Camera::y() const
{
	return y_;
}