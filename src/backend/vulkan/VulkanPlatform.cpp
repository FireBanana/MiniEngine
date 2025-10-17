#include "FrameGraph.h"
#include "VulkanPlatform.h"
#include "VulkanRenderDoc.h"

void MiniEngine::Backend::VulkanPlatform::initialize(
    MiniEngine::Types::EngineInitParams &params, Engine *engine)
{
    mParams = params;

    createDriver(params);
    createWindow(params.screenWidth, params.screenHeight);
}

void MiniEngine::Backend::VulkanPlatform::createWindow(uint16_t width, uint16_t height)
{
    MiniEngine::Logger::print("Initializing Vulkan Window...");

    if (!glfwInit())
        MiniEngine::Logger::eprint("GLFW not initialized...");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(width, height, "MiniEngine", NULL, NULL);
    if (!mWindow) {
        glfwTerminate();
        MiniEngine::Logger::eprint("Create window failed");
    }

    // Events
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);

    mDriver->initialize(mParams);

    uint32_t countExtensions;
    auto exts = glfwGetRequiredInstanceExtensions(&countExtensions);
    std::vector<const char *> extensions(countExtensions);

    for (int i = 0; i < countExtensions; ++i)
        extensions[i] = exts[i];

    extensions.push_back("VK_EXT_swapchain_colorspace");
    extensions.push_back("VK_KHR_get_physical_device_properties2");

#ifdef GRAPHICS_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    mDriver->createInstance(extensions, {"VK_LAYER_KHRONOS_validation"});

    VulkanRenderDoc::initRenderDoc();
#else
    mDriver->createInstance(extensions, {});
#endif

    glfwCreateWindowSurface(mDriver->getInstance(), mWindow, nullptr, &mSurface);

    mDriver->updateSurface(mSurface);
    mDriver->generateDevice();
    mDriver->generateSwapchain();
    mDriver->generateGbuffer();
    mDriver->generatePipelines();

    glfwMakeContextCurrent(mWindow);
}

void MiniEngine::Backend::VulkanPlatform::createDriver(MiniEngine::Types::EngineInitParams &params)
{
    mDriver = std::make_unique<VulkanDriver>();
}

void MiniEngine::Backend::VulkanPlatform::makeCurrent() {}

void MiniEngine::Backend::VulkanPlatform::execute(Scene *scene)
{
    FrameGraph mDefaultFrameGraph{};

    auto imageCache = mDriver->getImageCache();

    MiniEngine::Backend::TextureResourceDesc renderTexture{};
    renderTexture.type = Backend::TextureResourceDesc::Type::RENDER_TARGET;
    MiniEngine::Backend::TextureResourceDesc diffuse{}; //TODO FIX
    diffuse.type = Backend::TextureResourceDesc::Type::EXTERNAL;
    diffuse.image = &std::get<0>(imageCache[0]);

    // mDriver->syncTextures(scene);

    mDefaultFrameGraph
        .addPass("pass1", {"test", {diffuse}, {renderTexture}, {}, {}}, [this, scene]() {
            // Main pass
            mDriver->recordCommandBuffers(scene);
        });

    mDefaultFrameGraph.bake(scene);

    while (!glfwWindowShouldClose(mWindow)) //run separate thread
    {
        mDriver->draw(scene);
        glfwPollEvents();
    }
}

MiniEngine::Backend::IImgui *MiniEngine::Backend::VulkanPlatform::getUiInterface() const
{
    return nullptr;
}

void MiniEngine::Backend::VulkanPlatform::createImguiInterface() {}

void MiniEngine::Backend::VulkanPlatform::framebufferSizeCallback(
    GLFWwindow *window, int height, int width)
{
    MiniEngine::Logger::print("Resizing window");
}
