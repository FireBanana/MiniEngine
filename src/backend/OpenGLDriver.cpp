#include "OpenGLDriver.h"

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
    GLuint color, depth;
    glCreateFramebuffers(1, &mMainFrameBuffer);
    glCreateRenderbuffers(1, &color);
    glCreateRenderbuffers(1, &depth);

    glNamedRenderbufferStorage(color, GL_RGBA16F, mWidth, mHeight);
    glNamedFramebufferRenderbuffer(mMainFrameBuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color);

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

void OpenGLDriver::draw()
{
    // Mesh
}

void OpenGLDriver::finalBlit()
{
    constexpr GLenum attach[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_STENCIL_ATTACHMENT };

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mMainFrameBuffer);

    glBlitFramebuffer(0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, attach);
}

void OpenGLDriver::beginRenderpass()
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, mWidth, mHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, mMainFrameBuffer);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLDriver::endRenderpass()
{
    glDisable(GL_SCISSOR_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


