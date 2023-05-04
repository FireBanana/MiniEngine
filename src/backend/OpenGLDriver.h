#pragma once
#include "../core/types/EngineTypes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class Scene; //remove
class Shader;
class MeshComponent;

class OpenGLDriver
{
public:

	enum class ShaderType : GLenum
	{
		VERTEX  = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	void setupGlWindowParams(const EngineInitParams& params);
	void setupFrameBuffer();
	void setupDebugInfo();

	void setupMesh(MeshComponent* component);

	void draw(Scene* scene);
	void finalBlit();

	void beginRenderpass();
	void endRenderpass();

	unsigned int loadShader(const char* path, ShaderType type) const;
	unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const;
	void		 useShaderProgram(unsigned int program) const;

	void registerUniformBlock(const char* blockName, const Shader* program) const;
	void createUniformBlock(Shader* program, size_t dataSize, void* data) const;

	unsigned int createTexture(int width, int height, void* data);
	void bindTextureUnit(unsigned int texId, unsigned int bindUnit);

private:

	GLuint mMainFrameBuffer;
	
	uint16_t mWidth;
	uint16_t mHeight;
};