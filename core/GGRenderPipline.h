#ifndef _GG_RENDER_PIPLINE_H_
#define _GG_RENDER_PIPLINE_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "GGUniformManager.h"
#include "GGVaoManager.h"
#include "GGShaderManager.h"

namespace Gange {
class GGRenderPipline {
public:
    explicit GGRenderPipline(VkRenderPass renderPass, VkSampleCountFlagBits sampleCount);
    ~GGRenderPipline();

    void initialize();

	void buildCommandBuffers(VkCommandBuffer commandBuffer);

	void update();

    std::vector<VkPipeline> mPiplines;

    VkRenderPass mRenderPass;

	UniformManager *mUniformBufferManger = nullptr;

	VaoManager *mVaoManager = nullptr;

	ShaderManager* mShaderManager = nullptr;

    VkSampleCountFlagBits mSampleCount;

    VkPipelineCache mPipelineCache = VK_NULL_HANDLE;

	GGUniformBufferHandle* uniformBuffer;
};

}  // namespace Gange

#endif
