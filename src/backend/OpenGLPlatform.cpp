#include "OpenGLPlatform.h"
#include "../core/Scene.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

OpenGLPlatform::OpenGLPlatform(const EngineInitParams& params, Engine* engine) :
    mEngine(engine)
{
    createWindow(params.screenWidth, params.screenHeight);
    createDriver(params);
}

void OpenGLPlatform::createWindow(uint16_t width, uint16_t height)
{
    if (!glfwInit())
        std::cout << "Not inited";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    mWindow = glfwCreateWindow(width, height, "MiniEngine", NULL, NULL);
    if (!mWindow)
    {
        glfwTerminate();
        std::cout << "Create window failed";
    }

    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
    }
}

void OpenGLPlatform::makeCurrent()
{
    glfwMakeContextCurrent(mWindow);
}

void OpenGLPlatform::execute(Scene* scene)
{
    while (!glfwWindowShouldClose(mWindow)) //run separate thread
    {
        mDriver->beginRenderpass();
        mDriver->draw(scene);
        mDriver->endRenderpass();
        
        mDriver->finalBlit();

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
}

void OpenGLPlatform::createDriver(const EngineInitParams& params)
{
    mDriver = std::make_unique<OpenGLDriver>();

    mDriver->setupDebugInfo();
    mDriver->setupGlWindowParams(params, mEngine);
    mDriver->setupFrameBuffer();
    mDriver->setupScreenQuad();
}
