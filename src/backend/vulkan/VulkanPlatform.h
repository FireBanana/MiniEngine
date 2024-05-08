#pragma once

#include <memory>

#include "EngineTypes.h"
#include "IImgui.h"
#include "VulkanDriver.h"
#include "VulkanHelper.h"
#include "VulkanPlatform.h"
#include <GLFW/glfw3.h>

namespace MiniEngine
{

class Scene;
class Engine;

namespace Backend
{
class VulkanPlatform
{
public:
    void initialize(MiniEngine::Types::EngineInitParams &params, Engine *engine);
    void makeCurrent();
    void execute(Scene *scene);
    IImgui *getUiInterface() const;
    void createImguiInterface();

    VulkanDriver *getDriver() const { return mDriver.get(); }

private:
    std::unique_ptr<VulkanDriver> mDriver;
    GLFWwindow *mWindow;
    VkSurfaceKHR mSurface;
    Engine *mEngine;
    MiniEngine::Types::EngineInitParams mParams;

    void createWindow(uint16_t width, uint16_t height);
    void createDriver(MiniEngine::Types::EngineInitParams &params);
	};
}
}
