#pragma once

#include <vector>
#include "VulkanHelper.h"
#include "IDriver.h"

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

	class VulkanDriver : public IDriver
	{
	public:

		inline const VkInstance& getInstance() const { return mInstance; }

		void initialize();

		void createInstance(
			const std::vector<const char*>& requireInstanceExtensions,
			const std::vector<const char*>&& requiredLayers);

		unsigned int createTexture(int width, int height, int channels, void* data, Texture::TextureType type) override;
		unsigned int createUniformBlock(size_t dataSize, unsigned int bindIndex) const override;
		void updateUniformData(unsigned int bufferId, unsigned int offset, size_t size, void* data) const override;
		void registerUniformBlock(const char* blockName, const Shader* program, unsigned int layoutIndex) const override;
		void setupMesh(MiniEngine::Components::RenderableComponent* component) override;
		void setupSkybox(MiniEngine::Components::SkyboxComponent* skybox) override;
		unsigned int loadShader(const char* path, ShaderType type) const override;
		unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const override;
		void useShaderProgram(unsigned int program) const override;
		void setFloat(unsigned int program, const char* name, float value) const override;
		void setVec3(unsigned int program, const char* name, Vector3 value) const override;
		void setMat4(unsigned int program, const char* name, Matrix4x4 value) const override;

	private:

		VkInstance mInstance;

		void enumerateInstanceExtensionProperties();
		void enumerateInstanceLayerProperties();

	};
}
}
