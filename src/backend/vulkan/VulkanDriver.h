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

        inline const VkInstance &getInstance() const { return mInstance; }

        void initialize(MiniEngine::Types::EngineInitParams &params);

        void createInstance(
			const std::vector<const char*>& requireInstanceExtensions,
			const std::vector<const char*>&& requiredLayers);

		void generateDevice();
		void generateSwapchain();
		void generateRenderPass();
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
		VkRenderPass mDefaultRenderpass;
		VkPipeline mGbufferPipeline;
		VkPipeline mLightingPipeline;
		VkFormat mCurrentSwapchainFormat;
		VkFormat mCurrentSwapchainDepthFormat;
		int32_t mActiveQueue{ -1 };
		VkPhysicalDeviceMemoryProperties mGpuMemoryProperties;

		std::vector<VkFramebuffer> mFramebuffers;
		std::vector<PerFrameData>  mSwapchainPerImageData;
		std::array<VkImageView, 6> mFrameBufferAttachments;
        Utils::DynamicArray<DisplaySemaphore> mDisplaySemaphoreArray;

        void enumerateInstanceExtensionProperties();
        void enumerateInstanceLayerProperties();
		void enumerateDeviceExtensionProperties();
		void getPhysicalDevice();
		void getPhysicalDeviceQueueFamily();
		void createDevice(const std::vector<const char*>&& requiredExtensions);
		void createSwapchain();
		void createSwapchainImageViews();
		void createRenderPasses();
		void createGBufferPipeline();
		void createLightingPipeline();
		void createFramebuffer();
		void createFramebufferAttachmentSampler();
        void createDisplaySemaphores();
        void recordCommandBuffers();

        VkImageView createImageAttachment(VkFormat imageFormat,
                                          VkImageUsageFlags imageBits,
                                          VkImageAspectFlags imageViewAspectFlags);

        void loadShaderModule();
        void acquireNextImage(uint32_t *image, uint32_t *displaySemaphoreIndex);
        uint32_t getMemoryTypeIndex(const VkMemoryRequirements *memReqs);
    };
}
}
