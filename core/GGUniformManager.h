#ifndef _GG_TEXTURE_MANAGER_H_
#define _GG_TEXTURE_MANAGER_H_
#include "GGSingleton.h"
#include "Buffer/GGTextureImage.h"
#include "Buffer/GGUniformBufferHandle.h"
#include "GGVulkanDevice.h"
#include <vector>

namespace Gange {

class UniformManager {

public:

    UniformManager();

    ~UniformManager();

	void initialize();

	void update();

	VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;

	std::vector<GGTextureImage*>mTextureVec;

	std::vector<GGUniformBufferHandle*>mUniformBufferVec;
private:

	virtual void setupDescriptorSetLayout();

	virtual void setupDescriptorSet();

	std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;

	VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;

	std::vector<VkQueue>mQueueVec;

	GGVulkanDevice *mVulkanDevice = nullptr;

	
};

}  // namespace Gange

#endif