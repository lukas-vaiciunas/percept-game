#pragma once

class AABB;

class Camera
{
private:
	float x_;
	float y_;
public:
	Camera(float x, float y);

	void centerOn(float x, float y);
	void constrain(const AABB &bounds);

	float x() const;
	float y() const;
};