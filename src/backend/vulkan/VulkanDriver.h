#pragma once

namespace MiniEngine
{
class Scene;
class Shader;
class Engine;

namespace Components
{
	struct RenderableComponent;
	struct SkyboxComponent;
}

namespace Backend
{

	class VulkanDriver
	{
	public:

		void initialize();

		void createInstance();
		void enumerateInstanceExtensionProperties();

	private:


	};
}
}
