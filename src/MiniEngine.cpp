#include "core/Engine.h"
#include "core/Mesh.h"

int main(void)
{
	EngineInitParams params;
	params.screenWidth = 800;
	params.screenHeight = 800;
	params.clearColor = { 0.2f, 0.6f, 0.5f, 1.0f };

	Engine engine{ params };

	auto mesh = Mesh{};

	engine.execute();
}
