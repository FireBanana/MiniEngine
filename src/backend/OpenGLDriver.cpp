#include "OpenGLDriver.h"
#include "../core/utils/FileHelper.h"
#include "../core/Engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

void OpenGLDriver::setupGlWindowParams(const EngineInitParams& params)
{
    mWidth = params.screenWidth;
    mHeight = params.screenHeight;

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
    GLuint color, depth, position, normal, roughness;
    glCreateFramebuffers(1, &mMainFrameBuffer);
    glCreateRenderbuffers(1, &color);
    glCreateRenderbuffers(1, &depth);
    glCreateRenderbuffers(1, &position);
    glCreateRenderbuffers(1, &normal);
    glCreateRenderbuffers(1, &roughness);

    glNamedRenderbufferStorage(color, GL_RGBA16F, mWidth, mHeight);
    glNamedFramebufferRenderbuffer(mMainFrameBuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color);

    glNamedRenderbufferStorage(position, GL_RGBA16F, mWidth, mHeight);
    glNamedFramebufferRenderbuffer(mMainFrameBuffer, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, position);

    glNamedRenderbufferStorage(normal, GL_RGBA16F, mWidth, mHeight);
    glNamedFramebufferRenderbuffer(mMainFrameBuffer, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, normal);

    glNamedRenderbufferStorage(roughness, GL_RGBA16F, mWidth, mHeight);
    glNamedFramebufferRenderbuffer(mMainFrameBuffer, GL_COLOR_ATTACHMENT3, GL_RENDERBUFFER, roughness);

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

void OpenGLDriver::draw(Engine* engine)
{
    static bool once = false;

    static unsigned int vao, vbo, ebo;

    if (!once)
    {
        once = true;

        auto db = engine->getMeshComponentDatabase();

        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);
        glCreateBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        glNamedBufferData(
            vbo, db->at(0).vertices.size() * sizeof(float), db->at(0).vertices.data(), GL_STATIC_DRAW);
        glNamedBufferData(
            ebo, db->at(0).indices.size() * sizeof(unsigned int), db->at(0).indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    }
    glBindVertexArray(vao);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenGLDriver::finalBlit()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mMainFrameBuffer);

    glBlitFramebuffer(0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void OpenGLDriver::beginRenderpass()
{
    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, mWidth, mHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, mMainFrameBuffer);
    glNamedFramebufferDrawBuffers(mMainFrameBuffer, 4, attachments);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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


