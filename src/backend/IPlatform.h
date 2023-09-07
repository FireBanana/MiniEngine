#pragma once
#include <cstdint>
#include <EngineTypes.h>

namespace MiniEngine
{
	class Scene;
	class Engine;

namespace Backend
{
	class IDriver;
	class IImgui;

	class IPlatform
	{
	public:
		virtual void initialize(MiniEngine::Types::EngineInitParams& params, Engine* engine) = 0;

		virtual void makeCurrent() = 0;
		virtual void execute(Scene* scene) = 0;
		virtual IDriver* getDriver() const = 0;
		virtual IImgui* getUiInterface() const = 0;

	private:

		virtual void createWindow(uint16_t width, uint16_t height) = 0;
		virtual void createDriver(MiniEngine::Types::EngineInitParams& params) = 0;
		virtual void createImguiInterface() = 0;
	};
}
}