#pragma once
#include "../Camera.h"

class IDrawable
{
public:

	virtual void Draw(Camera camera) = 0;

};