#pragma once

#include <unordered_set>
#ifndef MINIENGINE_VULKAN_DRIVER
#define MINIENGINE_VULKAN_DRIVER

#include "VulkanBuffer.h"
#include "VulkanHelper.h"
#include "VulkanImage.h"
#include "VulkanPipeline.h"
#include "VulkanSwapchain.h"
#include "types/EngineTypes.h"
#include <VulkanMemory.h>
#include <vector>

namespace MiniEngine {
class Scene;
class Shader;
class Engine;

namespace Components {
struct RenderableComponent;
struct SkyboxComponent;
} // namespace Components

namespace Backend {
class VulkanDriver
{
public:
    constexpr static int FRAMES_IN_FLIGHT = 2;

    enum class ShaderType { VERTEX, FRAGMENT };

    enum class ImageAttachmentType : unsigned int { DEPTH, SWAPCHAIN };

    enum class TextureType : int { Default = 0, CubeMap = 1 };

    inline const VkInstance &getInstance() const { return mInstance; }

    void initialize(MiniEngine::Types::EngineInitParams &params);

    void createInstance(
        const std::vector<const char *> &requireInstanceExtensions,
        const std::vector<const char *> &&requiredLayers);

    void generateDevice();
    void generateSwapchain();
    void generatePipelines();

    void generateGbuffer();

    inline void updateSurface(VkSurfaceKHR s) { mSurface = s; }

    Texture createTexture(int width, int height, int channels, void *data, TextureType type);
    unsigned int createUniformBlock(size_t dataSize, unsigned int bindIndex) const;
    void updateUniformData(unsigned int bufferId, unsigned int offset, size_t size, void *data) const;
    void registerUniformBlock(
        const char *blockName, const Shader *program, unsigned int layoutIndex) const;
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

    void markTextureForUpload(VulkanImage image);
    void syncTextures(MiniEngine::Scene *scene);

    const std::vector<std::tuple<VulkanImage, bool>> &getImageCache() const
    {
        return mVulkanImageCache;
    }

private:
    MiniEngine::Types::EngineInitParams mParams;

    VkInstance mInstance;
    VkPhysicalDevice mActiveGpu;
    VkSurfaceKHR mSurface;
    VkDevice mActiveDevice;
    VkQueue mActiveDeviceQueue;
    VulkanSwapchain mActiveSwapchain;
    VulkanPipeline mGbufferPipeline;
    VmaAllocator mMemoryAllocator;
    int32_t mActiveQueue{-1};
    VkPhysicalDeviceMemoryProperties mGpuMemoryProperties;
    VulkanImage mPlaceholderImage;

    uint64_t mCurrentFrame = 0;

    std::vector<std::tuple<VulkanImage, bool>> mVulkanImageCache;
    std::array<VulkanImage, 2> mMainFrameBuffer;
    std::vector<VkSemaphore> mPresentSemaphores;
    std::vector<VkSemaphore> mAcquireSemaphores;
    std::vector<VkFence> mPresentFences;
    std::array<VkDescriptorPool, 2> mDescriptorPools; //change to enum
    std::vector<VulkanImage> mUploadBuffer;

    void enumerateInstanceExtensionProperties();
    void enumerateInstanceLayerProperties();
    void enumerateDeviceExtensionProperties();
    void registerPhysicalDevice();
    void registerPhysicalDeviceQueueFamily();
    void createDevice(const std::vector<const char *> &&requiredExtensions);
    void createSwapchain();
    void createDescriptorPools();
    void createGBufferPipeline();
    void createPresentSyncPrimitives();
    void initializeMemoryAllocator();
    void recordCommandBuffers(MiniEngine::Scene *scene, uint32_t imgIndex, int frameInFlightIndex);

    void loadShaderModule();
    void acquireNextImage(uint32_t frame, uint32_t *img);
    uint32_t getMemoryTypeIndex(const VkMemoryRequirements *memReqs);

    VulkanBuffer createBuffer(
        size_t memSize, void *data, VkBufferUsageFlags usageFlags, std::string &&debugName = "");

    friend class VulkanPipeline;
    friend class VulkanDescriptorSet;
    friend class VulkanImage;
    friend class VulkanSwapchain;
    friend class VulkanBuffer;
};
} // namespace Backend
} // namespace MiniEngine

#endif //MINIENGINE_VULKAN_DRIVER
