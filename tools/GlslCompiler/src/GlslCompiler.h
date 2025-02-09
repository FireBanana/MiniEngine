#pragma once
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <volk.h>

namespace MiniTools
{
namespace GlslCompiler
{
	VkShaderModule loadShader(std::string path, VkShaderStageFlagBits stage, VkDevice& device);
}
}
