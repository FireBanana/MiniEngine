#pragma once

#include "IDriver.h"
#include "VulkanHelper.h"
#include "VulkanPipelineBuilder.h"
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

	class VulkanDriver : public IDriver
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

        enum class ImageAttachmentType : unsigned int {
            COLOR,
            POSITION,
            NORMAL,
            ROUGHNESS,
            DEPTH,
            SWAPCHAIN
        };

        inline const VkInstance &getInstance() const { return mInstance; }

        void initialize(MiniEngine::Types::EngineInitParams &params);

        void createInstance(
			const std::vector<const char*>& requireInstanceExtensions,
			const std::vector<const char*>&& requiredLayers);

		void generateDevice();
		void generateSwapchain();
		void generatePipelines();
		
		void generateGbuffer();

		inline void updateSurface(VkSurfaceKHR s) { mSurface = s; }

		unsigned int createTexture(int width, int height, int channels, void* data, Texture::TextureType type) override;
		unsigned int createUniformBlock(size_t dataSize, unsigned int bindIndex) const override;
		void updateUniformData(unsigned int bufferId, unsigned int offset, size_t size, void* data) const override;
		void registerUniformBlock(const char* blockName, const Shader* program, unsigned int layoutIndex) const override;
		void setupMesh(MiniEngine::Components::RenderableComponent* component) override;
		void setupSkybox(MiniEngine::Components::SkyboxComponent* skybox) override;
		void beginRenderpass() override;
		void endRenderpass() override;
		void draw(MiniEngine::Scene* scene) override;
		unsigned int loadShader(const char* path, ShaderType type) const override;
		unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) const override;
		void useShaderProgram(unsigned int program) const override;
		void setFloat(unsigned int program, const char* name, float value) const override;
		void setVec3(unsigned int program, const char* name, Vector3 value) const override;
		void setMat4(unsigned int program, const char* name, Matrix4x4 value) const override;

	private:

		MiniEngine::Types::EngineInitParams mParams;

		std::unique_ptr<VulkanPipelineBuilder> mPipelineBuilder;

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
		int32_t mActiveQueue{ -1 };
		VkPhysicalDeviceMemoryProperties mGpuMemoryProperties;

        std::vector<PerFrameData>  mSwapchainPerImageData;
        std::array<ImageAttachmentData, 5> mImageAttachments;
        Utils::DynamicArray<DisplaySemaphore> mDisplaySemaphoreArray;

        void enumerateInstanceExtensionProperties();
        void enumerateInstanceLayerProperties();
		void enumerateDeviceExtensionProperties();
		void getPhysicalDevice();
		void getPhysicalDeviceQueueFamily();
		void createDevice(const std::vector<const char*>&& requiredExtensions);
		void createSwapchain();
		void createSwapchainImageViews();
		void createGBufferPipeline();
		void createLightingPipeline();
        void createDisplaySemaphores();
        void recordCommandBuffers();

        ImageAttachmentData createImageAttachment(VkFormat imageFormat,
                                                  VkImageUsageFlags imageBits,
                                                  VkImageAspectFlags imageViewAspectFlags);

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
    };
}
}
