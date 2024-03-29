#pragma once
#include "EngineTypes.h"
#include "Texture.h"
#include "EngineConstants.h"
#include "OpenGLImgui.h"
#include "IDriver.h"

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
	struct RenderableComponent;
	struct SkyboxComponent;
}

namespace Backend
{

	class OpenGLDriver : public IDriver
	{
	public:

		void setupGlWindowParams(MiniEngine::Types::EngineInitParams& params, Engine* engine);
		void setupFrameBuffer();
		void setupScreenQuad();
		void setupDebugInfo();
		void setupImguiInterface(OpenGLImgui* imguiInterface);

		void setupSkybox(MiniEngine::Components::SkyboxComponent* skybox) override;
		void setupMesh(MiniEngine::Components::RenderableComponent* component) override;
		void setupDefaultMaps();

		void draw(MiniEngine::Scene* scene);
		void finalBlit();

		void beginRenderpass();
		void endRenderpass();

		unsigned int loadShader(const char* path, ShaderType type) const override;
		unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const override; //todo: create deleter
		void		 useShaderProgram(unsigned int program) const override;

		unsigned int createUniformBlock(size_t dataSize, unsigned int bindIndex) const override;
		void registerUniformBlock(const char* blockName, const Shader* program, unsigned int layoutIndex) const override;
		void updateUniformData(unsigned int bufferId, unsigned int offset, size_t size, void* data) const override;

		unsigned int createTexture(int width, int height, int channels, void* data, Texture::TextureType type) override;

		void setFloat(unsigned int program, const char* name, float value) const override;
		void setVec3(unsigned int program, const char* name, Vector3 value) const override;
		void setMat4(unsigned int program, const char* name, Matrix4x4 value) const override;

	private:

		Types::EngineInitParams* mParams;

		Engine* mEngine;
		OpenGLImgui* mOpenGlImgui;

		GLuint mMainFrameBuffer;
		GLuint mAccumBuffer1;
		GLuint mAccumBuffer2;
		GLuint mColorBuffer;
		GLuint mPositionBuffer;
		GLuint mNormalBuffer;
		GLuint mRoughnessBuffer;
		GLuint mScreenQuadVertexArray;
		GLuint mDefaultFrameQuery;

		GLuint mDefaultMaps[MAX_TEXTURE_TYPES];

		uint16_t mWidth;
		uint16_t mHeight;
	};
}
}