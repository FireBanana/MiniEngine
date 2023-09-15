#pragma once

namespace MiniTools
{
namespace GlslCompiler
{
	VkShaderModule loadShader(std::string path, VkShaderStageFlagBits stage, VkDevice& device);
}
}