#include <volk.h>
#include "FileHelper.h"
#include "GlslCompiler.h"
#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

VkShaderModule MiniTools::GlslCompiler::loadShader(std::string path, VkShaderStageFlagBits stage, VkDevice& device)
{
	glslang::EShTargetLanguage targetLang = glslang::EShTargetLanguage::EShTargetNone;;
	std::vector<uint32_t> spirv;

	glslang::InitializeProcess();

	EShMessages messageFilter{ EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules };
	EShLanguage language{ stage == VK_SHADER_STAGE_VERTEX_BIT ? EShLangVertex : EShLangFragment };
	
	auto source = FileHelper::loadFile(path.c_str());

	const char* fileNameList[1] = { "" };
	auto shaderSource = reinterpret_cast<const char*>(source.data());

	glslang::TShader shader{ language };
	shader.setStringsWithLengthsAndNames(&shaderSource, nullptr, fileNameList, 1);
	shader.setEntryPoint("main");
	shader.setSourceEntryPoint("main");

	if (targetLang != glslang::EShTargetLanguage::EShTargetNone)
	{
		shader.setEnvTarget(targetLang, static_cast<glslang::EShTargetLanguageVersion>(0));
	}

	// Can pass include dir here
	if (!shader.parse(GetDefaultResources(), 100, false, messageFilter))
	{
		auto shaderLog = std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
		MiniEngine::Logger::print(shaderLog);
		throw;
	}

	glslang::TProgram program;
	program.addShader(&shader);

	if (!program.link(messageFilter))
	{
		auto shaderLog = std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
		MiniEngine::Logger::print(shaderLog);
		throw;
	}

	// Save any info log that was generated.
	if (shader.getInfoLog())
	{
		auto logs = std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog()) + "\n";
	}

	if (program.getInfoLog())
	{
		auto logs = std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());
	}

	glslang::TIntermediate* intermediate = program.getIntermediate(language);

	if (!intermediate)
	{
		throw;
	}

	spv::SpvBuildLogger logger;

	glslang::GlslangToSpv(*intermediate, spirv, &logger);

	auto log = logger.getAllMessages();

	glslang::FinalizeProcess();

	VkShaderModuleCreateInfo moduleInfo{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	moduleInfo.codeSize = spirv.size() * sizeof(uint32_t);
	moduleInfo.pCode = spirv.data();

	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &moduleInfo, nullptr, &shaderModule);

	return shaderModule;
}
