#ifndef _GANGE_SHADER_MANAGER_H_
#define _GANGE_SHADER_MANAGER_H_
#include <string>
#include <vulkan/vulkan.h>
#include <vector>

namespace Gange {
class ShaderManager {
public:
	ShaderManager();
	
	~ShaderManager();

	std::vector<VkPipelineShaderStageCreateInfo> &getShaderStages();

private:

	void setupShader();

	VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);

	std::vector<VkPipelineShaderStageCreateInfo> mShaderStages;

};
}  // namespace Gange

#endif
