#include "VulkanPlatform.h"
#include "Logger.h"
#include "VulkanHelper.h"

MiniEngine::Backend::VulkanPlatform::VulkanPlatform(MiniEngine::Types::EngineInitParams& params, Engine* engine)
{
}

void MiniEngine::Backend::VulkanPlatform::createWindow(uint16_t width, uint16_t height)
{
    MiniEngine::Logger::print("Initializing Vulkan Window...");

    if (!glfwInit())
        MiniEngine::Logger::eprint("GLFW not initialized...");


    mWindow = glfwCreateWindow(width, height, "MiniEngine", NULL, NULL);
    if (!mWindow)
    {
        glfwTerminate();
        MiniEngine::Logger::eprint("Create window failed");
    }

    mDriver->initialize();

    //VK_CHECK(glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface));

    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);
}

void MiniEngine::Backend::VulkanPlatform::createDriver(MiniEngine::Types::EngineInitParams& params)
{
    mDriver = std::make_unique<VulkanDriver>();
}
