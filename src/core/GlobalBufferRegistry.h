#pragma once

#include <vector>

#include "../backend/OpenGLDriver.h"
#include "types/EngineTypes.h"

namespace MiniEngine
{
	class GlobalBufferRegistry
	{
	public:

		enum class BlockType
		{
			SceneBlock
		};

		struct SceneBlock
		{
			// Camera
			Matrix4x4 view;
			Matrix4x4 projection;
			Vector3 cameraPos;

			// Lights
			Vector3 lightPos1;
			float lightIntensity1;

			//Material
			float roughness;
			float metallic;
		};

		GlobalBufferRegistry(const Backend::OpenGLDriver* driver);

		void createNewBinding(BlockType block, unsigned int bindIndex);
		void updateUniformData(BlockType block, unsigned int offset, size_t size, void* data);
		
		inline SceneBlock* getSceneBlockInstance() { return &mSceneBlockInstance; }

	private:

		const Backend::OpenGLDriver* mDriver;
		std::unordered_map<BlockType, unsigned int> mBindingPoints;
		SceneBlock mSceneBlockInstance;

		size_t parseBlockSize(BlockType type);
	};
}