#pragma once
#include "../Camera.h"

class IDrawable abstract
{
public:

	virtual void Draw(Camera camera) = 0;

};