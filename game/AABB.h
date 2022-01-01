#pragma once

class AABB
{
private:
	float x_;
	float y_;
	float width_;
	float height_;
protected:
	void setX_(float x);
	void setY_(float y);
	void moveX_(float dX);
	void moveY_(float dY);
public:
	AABB(float x, float y, float width, float height);
	virtual	~AABB() {}

	float x() const;
	float y() const;
	float width() const;
	float height() const;
};