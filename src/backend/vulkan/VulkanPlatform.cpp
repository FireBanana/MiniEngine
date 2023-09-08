#include "VulkanPlatform.h"

void MiniEngine::Backend::VulkanPlatform::initialize(MiniEngine::Types::EngineInitParams& params, Engine* engine)
{
    createDriver(params);
    createWindow(params.screenWidth, params.screenWidth);
}

void MiniEngine::Backend::VulkanPlatform::createWindow(uint16_t width, uint16_t height)
{
    MiniEngine::Logger::print("Initializing Vulkan Window...");

    if (!glfwInit())
        MiniEngine::Logger::eprint("GLFW not initialized...");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(width, height, "MiniEngine", NULL, NULL);
    if (!mWindow)
    {
        glfwTerminate();
        MiniEngine::Logger::eprint("Create window failed");
    }

    mDriver->initialize();

    uint32_t countExtensions;
    auto exts = glfwGetRequiredInstanceExtensions(&countExtensions);
    std::vector<const char*> extensions(countExtensions);

    for (int i = 0; i < countExtensions; ++i) extensions[i] = exts[i];

    mDriver->createInstance(extensions, {});

    VK_CHECK(glfwCreateWindowSurface(mDriver->getInstance(), mWindow, nullptr, &mSurface));

    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);
}

void MiniEngine::Backend::VulkanPlatform::createDriver(MiniEngine::Types::EngineInitParams& params)
{
    mDriver = std::make_unique<VulkanDriver>();
}

void MiniEngine::Backend::VulkanPlatform::makeCurrent()
{
}

void MiniEngine::Backend::VulkanPlatform::execute(Scene* scene)
{
    while (!glfwWindowShouldClose(mWindow)) //run separate thread
    {
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
}

MiniEngine::Backend::IImgui* MiniEngine::Backend::VulkanPlatform::getUiInterface() const
{
    return nullptr;
}

void MiniEngine::Backend::VulkanPlatform::createImguiInterface()
{
}
