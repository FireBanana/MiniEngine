#include "OpenGLDriver.h"
#include "../core/utils/FileHelper.h"
#include "../core/Scene.h"
#include "../components/RenderableComponent.h"
#include "../core/Shader.h"
#include "../core/Engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>

void debugCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
    auto const src_str = [source]() {
        switch (source)
        {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        }
    }();

    auto const type_str = [type]() {
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        }
    }();
    std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

void OpenGLDriver::setupGlWindowParams(const EngineInitParams& params, Engine* engine)
{
    mWidth = params.screenWidth;
    mHeight = params.screenHeight;

    mEngine = engine;

    glViewport(0, 0, mWidth, mHeight);

    glClearColor(
        params.clearColor.r(),
        params.clearColor.g(),
        params.clearColor.b(),
        params.clearColor.a()
    );

    glEnable(GL_DEPTH_TEST);
}

void OpenGLDriver::setupFrameBuffer()
{
    GLuint depth;
    glCreateFramebuffers(1, &mMainFrameBuffer);
    glCreateRenderbuffers(1, &depth);
    glCreateTextures(GL_TEXTURE_2D, 1, &mColorBuffer);
    glCreateTextures(GL_TEXTURE_2D, 1, &mPositionBuffer);
    glCreateTextures(GL_TEXTURE_2D, 1, &mNormalBuffer);
    glCreateTextures(GL_TEXTURE_2D, 1, &mRoughnessBuffer);
    glCreateTextures(GL_TEXTURE_2D, 1, &mAccumBuffer);

    glTextureStorage2D(mAccumBuffer, 1, GL_RGBA32F, mWidth, mHeight);
    glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT0, mAccumBuffer, 0);

    glTextureStorage2D(mColorBuffer, 1, GL_RGBA32F, mWidth, mHeight);
    glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT1, mColorBuffer, 0);

    glTextureStorage2D(mPositionBuffer, 1, GL_RGBA32F, mWidth, mHeight);
    glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT2, mPositionBuffer, 0);

    glTextureStorage2D(mNormalBuffer, 1, GL_RGBA32F, mWidth, mHeight);
    glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT3, mNormalBuffer, 0);

    glTextureStorage2D(mRoughnessBuffer, 1, GL_RGBA32F, mWidth, mHeight);
    glNamedFramebufferTexture(mMainFrameBuffer, GL_COLOR_ATTACHMENT4, mRoughnessBuffer, 0);

    glNamedRenderbufferStorage(depth, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    glNamedFramebufferRenderbuffer(mMainFrameBuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

    glBindFramebuffer(GL_FRAMEBUFFER, mMainFrameBuffer);

    if (glCheckNamedFramebufferStatus(mMainFrameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer not complete";
    }
}

void OpenGLDriver::setupDebugInfo()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugCallback, nullptr);
}

void OpenGLDriver::setupMesh(RenderableComponent* component)
{
    unsigned int vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    component->vaoId = vao;
    
    glBindVertexArray(vao);

    unsigned int vbo, ebo;

    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, component->stride * sizeof(float));
    glVertexArrayElementBuffer(vao, ebo);

    int start = 0;

    for (auto i = 0; i < component->attributes.size(); ++i)
    {
        glEnableVertexAttribArray(i);
        
        glVertexAttribFormat(i, component->attributes[i], GL_FLOAT, GL_FALSE, (start * sizeof(float)));
        glVertexAttribBinding(i, 0);

        start += component->attributes[i];
    }

    glNamedBufferData(
        vbo, component->buffer.size() * sizeof(float), component->buffer.data(), GL_STATIC_DRAW);
    glNamedBufferData(
        ebo, component->indices.size() * sizeof(unsigned int), component->indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void OpenGLDriver::draw(Scene* scene)
{
    const auto& db = scene->getRenderableComponentDatabase();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // geometry pass ==========================

    mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getDeferredShader());
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Diffuse"), 0);
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Position"), 1);
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Normal"), 2);
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Roughness"), 3);

    for (int i = 0; i < db.size(); ++i)
    {
        glBindVertexArray(db[i].vaoId);

        for (int j = 0; j < db[i].textures.size(); ++j)
        {
            glBindTextureUnit(j, db[i].textures[j].getId());
        }

        glDrawElements(GL_TRIANGLES, db[i].indices.size(), GL_UNSIGNED_INT, 0);

        for (int j = 0; j < db[i].textures.size(); ++j)
        {
            glBindTextureUnit(j, 0);
        }
    }

    auto mainPassSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    // light pass ==========================

    glBindTextureUnit(0, mColorBuffer);
    glBindTextureUnit(1, mPositionBuffer);
    glBindTextureUnit(2, mNormalBuffer);
    glBindTextureUnit(3, mRoughnessBuffer);

    mEngine->getShaderRegistry()->enable(mEngine->getShaderRegistry()->getPbrShader());
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Diffuse"), 0);
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Position"), 1);
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Normal"), 2);
    glUniform1i(glGetUniformLocation(mEngine->getShaderRegistry()->getActiveShader()->getShaderProgram(), "_Roughness"), 3);

    glBindVertexArray(mScreenQuadVertexArray);

    glWaitSync(mainPassSync, 0, GL_TIMEOUT_IGNORED);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glDeleteSync(mainPassSync);

    // unbound ============================
}

void OpenGLDriver::finalBlit()
{
    glBlitNamedFramebuffer(
        mMainFrameBuffer, 0,
        0, 0, mWidth, mHeight,
        0, 0, mWidth, mHeight,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void OpenGLDriver::beginRenderpass()
{
    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, mWidth, mHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, mMainFrameBuffer);
    glNamedFramebufferDrawBuffers(mMainFrameBuffer, 5, attachments);
}

void OpenGLDriver::endRenderpass()
{
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint OpenGLDriver::loadShader(const char* path, ShaderType type) const
{
    auto shaderFile = FileHelper::loadFile(path);
    auto shaderStr = shaderFile.c_str();
    auto shaderId = glCreateShader((GLenum)type);

    glShaderSource(shaderId, 1, &shaderStr, nullptr);
    glCompileShader(shaderId);

    int isCompiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);

    if (!isCompiled)
    {
        char log[512];
        glGetShaderInfoLog(shaderId, 512, nullptr, log);
        std::cout << "Shader compilation error: " << log;
    }

    return shaderId;
}

void OpenGLDriver::useShaderProgram(unsigned int program) const
{
    glUseProgram(program);
}

void OpenGLDriver::registerUniformBlock(const char* blockName, const Shader* program) const
{
    auto blockIndex = glGetUniformBlockIndex(program->getShaderProgram(), blockName);
    glUniformBlockBinding(program->getShaderProgram(), blockIndex, 0); //only 0 binding
}

void OpenGLDriver::createUniformBlock(Shader* program, size_t dataSize, void* data) const
{
    auto indexPtr = program->getUniformBlockBufferPoint();

    glCreateBuffers(1, indexPtr);
    glBindBuffer(GL_UNIFORM_BUFFER, *indexPtr);
    glBufferData(GL_UNIFORM_BUFFER, dataSize, data, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, *indexPtr); // only 0 binding
}

unsigned int OpenGLDriver::createTexture(int width, int height, void* data)
{
    GLuint tex;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    glTextureStorage2D(tex, 1, GL_RGB32F, width, height);
    glTextureSubImage2D(tex, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    return tex;
}

void OpenGLDriver::setupScreenQuad()
{
    constexpr float screenQuad[] =
    {
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f , 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f
    };

    constexpr unsigned int screenIndex[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    static GLuint sQuad, sEbo;

    glCreateVertexArrays(1, &mScreenQuadVertexArray);
    glBindVertexArray(mScreenQuadVertexArray);

    glCreateBuffers(1, &sQuad);
    glCreateBuffers(1, &sEbo);

    glVertexArrayVertexBuffer(mScreenQuadVertexArray, 0, sQuad, 0, 4 * sizeof(float));
    glVertexArrayElementBuffer(mScreenQuadVertexArray, sEbo);

    glEnableVertexAttribArray(0);
    glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
    glVertexAttribBinding(1, 0);

    glNamedBufferData(sQuad, sizeof(screenQuad), screenQuad, GL_STATIC_DRAW);
    glNamedBufferData(sEbo, sizeof(screenIndex), screenIndex, GL_STATIC_DRAW);
}

unsigned int OpenGLDriver::createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const
{
    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int isCompiled;
    glGetProgramiv(program, GL_LINK_STATUS, &isCompiled);

    if (!isCompiled)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cout << "Shader linking error: " << log;
    }

    return program;
}


