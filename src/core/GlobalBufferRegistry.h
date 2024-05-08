#pragma once

#include "VulkanDriver.h"
#include "types/EngineTypes.h"
#include <unordered_map>
#include <vector>

namespace MiniEngine
{
class GlobalBufferRegistry // moved to backend for now, remove
{
public:
    enum class BlockType { SceneBlock };

    struct SceneBlock
    {
        // Camera
        Matrix4x4 view;
        Matrix4x4 projection;
        Vector3 cameraPos;
        float p1; //padding

        // Lights
        Vector3 lightPos1;
        float lightIntensity1;
        float p3[3]; //padding

        //Material
        float roughness;
        float p4[3]; //padding
        float metallic;
        float p5[3]; //padding
    };

    GlobalBufferRegistry(const Backend::VulkanDriver *driver);

    void createNewBinding(BlockType block, unsigned int bindIndex);
    void updateUniformData(BlockType block, unsigned int offset, size_t size, void *data);

    inline SceneBlock *getSceneBlockInstance() { return &mSceneBlockInstance; }

private:
    const Backend::VulkanDriver *mDriver;
    std::unordered_map<BlockType, unsigned int> mBindingPoints;
    SceneBlock mSceneBlockInstance;

    size_t parseBlockSize(BlockType type);
	};
}
