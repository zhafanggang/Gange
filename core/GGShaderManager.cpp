#include "GGShaderManager.h"
#include "GGVulkanTools.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {

	ShaderManager::ShaderManager()
	{
		setupShader();
	}

	ShaderManager::~ShaderManager()
	{

	}

	std::vector<VkPipelineShaderStageCreateInfo> &ShaderManager::getShaderStages()
	{
		return mShaderStages;
	}

	void ShaderManager::setupShader()
	{
		mShaderStages.resize(2);

		mShaderStages[0] = loadShader(getAssetPath() + "shaders/phong/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
		mShaderStages[1] = loadShader(getAssetPath() + "shaders/phong/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	VkPipelineShaderStageCreateInfo ShaderManager::loadShader(std::string fileName, VkShaderStageFlagBits stage) {
		VkPipelineShaderStageCreateInfo shaderStage = {};
		shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStage.stage = stage;
#if defined(__ANDROID__)
		shaderStage.module = Vulkantools::loadShader(VulkanSingleHandle::getAssetManager(), fileName.c_str(), VulkanSingleHandle::getVulkanDevice().logicalDevice);
#else
		shaderStage.module = Vulkantools::loadShader(fileName.c_str(), VulkanSingleHandle::getVulkanDevice().logicalDevice);
#endif
		shaderStage.pName = "main";
		assert(shaderStage.module != VK_NULL_HANDLE);
		return shaderStage;
	}
}