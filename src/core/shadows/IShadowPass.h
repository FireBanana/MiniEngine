#pragma once

class Renderer;
class Camera;

class IShadowPass
{
public:

	unsigned int    ShadowMapId;
	unsigned int    ShadowBuffer;

	virtual void Setup() = 0;
	virtual void Pass(Renderer* renderer, Camera* camera) = 0;
};
