#include "OpenGLPlatform.h"
#include "../core/Scene.h"
#include "Logger.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

namespace MiniEngine::Backend
{
    void OpenGLPlatform::initialize(MiniEngine::Types::EngineInitParams& params, Engine* engine)
    {
        mEngine = engine;

        createWindow(params.screenWidth, params.screenHeight);
        createImguiInterface();
        createDriver(params);
    }

    void OpenGLPlatform::createWindow(uint16_t width, uint16_t height)
    {
        if (!glfwInit())
            MiniEngine::Logger::eprint("GLFW not initialized...");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        mWindow = glfwCreateWindow(width, height, "MiniEngine", NULL, NULL);
        if (!mWindow)
        {
            glfwTerminate();
            MiniEngine::Logger::eprint("Create window failed");
        }

        glfwMakeContextCurrent(mWindow);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
        {
            MiniEngine::Logger::eprint("Failed to initialize OpenGL context");
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

            mImgui->draw();

            glfwSwapBuffers(mWindow);
            glfwPollEvents();
        }
    }

    void OpenGLPlatform::createDriver(MiniEngine::Types::EngineInitParams & params)
    {
        mDriver = std::make_unique<OpenGLDriver>();

        mDriver->setupDebugInfo();
        mDriver->setupGlWindowParams(params, mEngine);
        mDriver->setupFrameBuffer();
        mDriver->setupScreenQuad();
        mDriver->setupDefaultMaps();
        mDriver->setupImguiInterface(mImgui.get());
    }

    void OpenGLPlatform::createImguiInterface()
    {
        mImgui = std::make_unique<OpenGLImgui>();
        mImgui->initialize(mWindow);
    }
}