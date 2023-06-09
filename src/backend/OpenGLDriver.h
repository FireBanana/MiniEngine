#pragma once
#include "../core/types/EngineTypes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace MiniEngine
{
	class Scene;
	class Shader;
	class Engine;

	namespace Components
	{
		class RenderableComponent;
	}

	namespace Backend
	{

		class OpenGLDriver
		{
		public:

			enum class ShaderType : GLenum
			{
				VERTEX = GL_VERTEX_SHADER,
				FRAGMENT = GL_FRAGMENT_SHADER
			};

			void setupGlWindowParams(const EngineInitParams& params, Engine* engine);
			void setupFrameBuffer();
			void setupScreenQuad();
			void setupDebugInfo();

			void setupMesh(MiniEngine::Components::RenderableComponent* component);

			void draw(MiniEngine::Scene* scene);
			void finalBlit();

			void beginRenderpass();
			void endRenderpass();

			unsigned int loadShader(const char* path, ShaderType type) const;
			unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const; //create deleter
			void		 useShaderProgram(unsigned int program) const;

			void registerUniformBlock(const char* blockName, const Shader* program, unsigned int bindIndex) const;
			unsigned int createUniformBlock(size_t dataSize, void* data, unsigned int bindIndex) const;

			unsigned int createTexture(int width, int height, int channels, void* data);

			void setFloat(unsigned int program, const char* name, float value) const;
			void setVec3(unsigned int program, const char* name, Vector3 value) const;
			void setMat4(unsigned int program, const char* name, Matrix4x4 value) const;

		private:

			Engine* mEngine;

			GLuint mMainFrameBuffer;
			GLuint mAccumBuffer;
			GLuint mColorBuffer;
			GLuint mPositionBuffer;
			GLuint mNormalBuffer;
			GLuint mRoughnessBuffer;
			GLuint mScreenQuadVertexArray;

			uint16_t mWidth;
			uint16_t mHeight;
		};
	}
}