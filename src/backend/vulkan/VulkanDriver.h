#ifndef MINIENGINE_VULKAN_DRIVER
#define MINIENGINE_VULKAN_DRIVER

#include "VulkanHelper.h"
#include "types/EngineTypes.h"
#include "utils/DynamicArray.h"
#include <vector>

constexpr VkFormat PREFERRED_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;

namespace MiniEngine
{
class Scene;
class Shader;
class Engine;

namespace Components
{
	struct RenderableComponent;
	struct SkyboxComponent;
}

namespace Backend
{

class VulkanPipelineBuilder;

class VulkanDriver
{
public:
    struct PerFrameData
    {
        VkImage rawImage;
        VkImageView imageView;
        VkCommandPool imageCommandPool;
        VkCommandBuffer imageCommandBuffer;
    };

    struct DisplaySemaphore
    {
        VkSemaphore acquisitionSemaphore;
        VkSemaphore presentationSemaphore;
        VkFence fence;
    };

    struct ImageAttachmentData
    {
        VkImage rawImage;
        VkImageView imageView;
    };

    enum class ShaderType { VERTEX, FRAGMENT };

    enum class ImageAttachmentType : unsigned int {
        COLOR,
        POSITION,
        NORMAL,
        ROUGHNESS,
        DEPTH,
        SWAPCHAIN
    };

    enum class TextureType : int { Default = 0, CubeMap = 1 };

    inline const VkInstance &getInstance() const { return mInstance; }

    void initialize(MiniEngine::Types::EngineInitParams &params);

    ~VulkanDriver();

    void createInstance(const std::vector<const char *> &requireInstanceExtensions,
                        const std::vector<const char *> &&requiredLayers);

    void generateDevice();
    void generateSwapchain();
    void generatePipelines();

    void generateGbuffer();

    inline void updateSurface(VkSurfaceKHR s) { mSurface = s; }

    unsigned int createTexture(int width, int height, int channels, void *data, TextureType type);
    unsigned int createUniformBlock(size_t dataSize, unsigned int bindIndex) const;
    void updateUniformData(unsigned int bufferId,
                           unsigned int offset,
                           size_t size,
                           void *data) const;
    void registerUniformBlock(const char *blockName,
                              const Shader *program,
                              unsigned int layoutIndex) const;
    void setupMesh(MiniEngine::Components::RenderableComponent *component);
    void setupSkybox(MiniEngine::Components::SkyboxComponent *skybox);
    void beginRenderpass();
    void endRenderpass();
    void draw(MiniEngine::Scene *scene);
    unsigned int loadShader(const char *path, ShaderType type) const;
    unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const;
    void useShaderProgram(unsigned int program) const;
    void setFloat(unsigned int program, const char *name, float value) const;
    void setVec3(unsigned int program, const char *name, Vector3 value) const;
    void setMat4(unsigned int program, const char *name, Matrix4x4 value) const;

private:
    MiniEngine::Types::EngineInitParams mParams;

    VulkanPipelineBuilder *mPipelineBuilder;

    VkInstance mInstance;
    VkPhysicalDevice mActiveGpu;
    VkSurfaceKHR mSurface;
    VkDevice mActiveDevice;
    VkQueue mActiveDeviceQueue;
    VkSwapchainKHR mActiveSwapchain;
    VkPipeline mGbufferPipeline;
    VkPipeline mLightingPipeline;
    VkPipelineLayout mDefaultPipelineLayout;
    VkFormat mCurrentSwapchainFormat;
    VkFormat mCurrentSwapchainDepthFormat;
    int32_t mActiveQueue{-1};
    VkPhysicalDeviceMemoryProperties mGpuMemoryProperties;

    std::vector<PerFrameData> mSwapchainPerImageData;
    std::array<ImageAttachmentData, 5> mImageAttachments;
    Utils::DynamicArray<DisplaySemaphore> mDisplaySemaphoreArray;

    void enumerateInstanceExtensionProperties();
    void enumerateInstanceLayerProperties();
    void enumerateDeviceExtensionProperties();
    void getPhysicalDevice();
    void getPhysicalDeviceQueueFamily();
    void createDevice(const std::vector<const char *> &&requiredExtensions);
    void createSwapchain();
    void createSwapchainImageViews();
    void createGBufferPipeline();
    void createLightingPipeline();
    void createDisplaySemaphores();
    void recordCommandBuffers();

    ImageAttachmentData createImageAttachment(VkFormat imageFormat,
                                              VkImageUsageFlags imageBits,
                                              VkImageAspectFlags imageViewAspectFlags,
                                              std::string debugName = {});

    void createPipelineBarrier(VkImage image,
                               VkCommandBuffer buffer,
                               VkAccessFlags srcAccessMask,
                               VkAccessFlags dstAccessMask,
                               VkImageAspectFlags aspectMask,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout,
                               VkPipelineStageFlags srcStageMask,
                               VkPipelineStageFlags dstStageMask);

    void loadShaderModule();
    void acquireNextImage(uint32_t *image, uint32_t *displaySemaphoreIndex);
    uint32_t getMemoryTypeIndex(const VkMemoryRequirements *memReqs);

    VkDeviceMemory allocateBuffer(VkBuffer buffer);
    void pushBufferMemory(VkBuffer buffer, VkDeviceMemory bufferMemory, void *data, size_t size);

    friend class VulkanPipelineBuilder;
};
}
}

#endif //MINIENGINE_VULKAN_DRIVER
