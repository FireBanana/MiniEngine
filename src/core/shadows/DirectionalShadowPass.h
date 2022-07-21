#pragma once

#include "IShadowPass.h"

class ShadowSettings;
class Renderer;
class Camera;

class DirectionalShadowPass : public IShadowPass
{
public:

	ShadowSettings* m_ShadowSettings;

	DirectionalShadowPass(ShadowSettings* settings);

	virtual void Setup() override;
	virtual void Pass(Renderer* renderer, Camera* camera) override;

};
