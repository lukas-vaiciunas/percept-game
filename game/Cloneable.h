#pragma once

class Cloneable
{
public:
	Cloneable() {}
	virtual ~Cloneable() {}

	virtual Cloneable *clone() const = 0;
};