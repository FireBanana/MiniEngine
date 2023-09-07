#pragma once

#include "Texture.h"
#include "EngineTypes.h"

#include <glad/glad.h>

namespace MiniEngine
{
	class Shader;

namespace Components
{
	struct RenderableComponent;
	struct SkyboxComponent;
}

namespace Backend
{

	class IDriver
	{
	public:

		enum class ShaderType
		{
#ifdef USING_OPENGL
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER
#else
			VERTEX,
			FRAGMENT
#endif
		};

		virtual unsigned int createTexture(int width, int height, int channels, void* data, Texture::TextureType type) = 0;
		virtual unsigned int createUniformBlock(size_t dataSize, unsigned int bindIndex) const = 0;

		virtual void updateUniformData(unsigned int bufferId, unsigned int offset, size_t size, void* data) const = 0;
		virtual void registerUniformBlock(const char* blockName, const Shader* program, unsigned int layoutIndex) const = 0;

		virtual void setupMesh(MiniEngine::Components::RenderableComponent* component) = 0;
		virtual void setupSkybox(MiniEngine::Components::SkyboxComponent* skybox) = 0;

		virtual unsigned int loadShader(const char* path, ShaderType type) const = 0;

		virtual unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const = 0;
		virtual void useShaderProgram(unsigned int program) const = 0;

		virtual void setFloat(unsigned int program, const char* name, float value) const = 0;
		virtual void setVec3(unsigned int program, const char* name, Vector3 value) const = 0;
		virtual void setMat4(unsigned int program, const char* name, Matrix4x4 value) const = 0;
	};

}
}