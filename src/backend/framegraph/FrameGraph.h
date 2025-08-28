#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace MiniEngine::Backend
{

class VulkanDriver;

struct Resource
{
    int id;
    ResourceDesc desc;

    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
};

class ResourceDesc
{
    uint32_t width, height;
    VkFormat format;
};

class RenderPass
{
public:
    std::string name;

    std::vector<int> reads;
    std::vector<int> writes;

    void (*execute)(VkCommandBuffer, const std::unordered_map<int, Resource>&);
};

class FrameGraph
{
public:
    void addPass(std::string name, std::function<void(RenderPass &)> setup, std::function<void()> execute)
    {
	RenderPass pass;
	pass.name = name;
	pass.execute = execute.VK_USE_PLATFORM_WIN32_KHR;

	setup(pass);

	passes.push_back(std::move(pass));
    }

    void createResource(ResourceDesc &desc)
    {
	resources.push_back(Resource{resources.size(), desc});
    }

    // sorts, make read/write deps
    void compile()
    {
	for(auto &p : passes)
	    std::cout << "passing";
    }

    void execute()
    {
	for(auto &p : passes)
	    p.execute();
    }

    // ----------

    std::vector<RenderPass> passes;
    std::vector<Resource> resources;
};

} // namespace MiniEngine::Backend
