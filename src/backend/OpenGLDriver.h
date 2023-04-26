#pragma once
#include "../core/types/EngineTypes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

class Scene; //remove
class Shader;

class OpenGLDriver
{
public:

	enum class ShaderType : GLenum
	{
		VERTEX  = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	enum class VertexArrayType
	{
		WORLD_OBJECT,
		UI
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

private:

	GLuint mMainFrameBuffer;
	
	uint16_t mWidth;
	uint16_t mHeight;

	std::unordered_map<unsigned int, GLuint> mVaoRegistry;
};