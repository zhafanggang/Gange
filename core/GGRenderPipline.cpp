#include "GGRenderPipline.h"
#include "GGVulkanInitializers.h"
#include "GGVulkanSingleHandle.h"
#include "GGVulkanGlTFModel.h"
#include "GGVulkanTools.h"

namespace Gange {
GGRenderPipline::GGRenderPipline(Renderable *renderable, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount) {
    mRenderable = renderable;
    mRenderPass = renderPass;
    mSampleCount = sampleCount;
}
GGRenderPipline::~GGRenderPipline() {}

void GGRenderPipline::initialize() {
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VK_CHECK_RESULT(vkCreatePipelineCache(GGVulkanSingleHandle::getVulkanDevice().logicalDevice,
                                          &pipelineCacheCreateInfo, nullptr, &mPipelineCache));

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI =
        initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
    VkPipelineRasterizationStateCreateInfo rasterizationStateCI = initializers::pipelineRasterizationStateCreateInfo(
        VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
    VkPipelineColorBlendAttachmentState blendAttachmentStateCI =
        initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
    VkPipelineColorBlendStateCreateInfo colorBlendStateCI =
        initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentStateCI);
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCI =
        initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
    VkPipelineViewportStateCreateInfo viewportStateCI = initializers::pipelineViewportStateCreateInfo(1, 1, 0);
    VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
    multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // Number of samples to use for rasterization
    multisampleStateCI.rasterizationSamples = mSampleCount;
#if GG_ENABLE_MULTI_SAMPLE
    VkPipelineMultisampleStateCreateInfo multisampleStateCI =
        initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
#endif
    const std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateCI = initializers::pipelineDynamicStateCreateInfo(
        dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);

    VkGraphicsPipelineCreateInfo pipelineCI =
        initializers::pipelineCreateInfo(mRenderable->mPipelineLayout, mRenderPass, 0);
    pipelineCI.pVertexInputState = &mRenderable->mVertexBuffer->mInputState;
    pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
    pipelineCI.pRasterizationState = &rasterizationStateCI;
    pipelineCI.pColorBlendState = &colorBlendStateCI;
    pipelineCI.pMultisampleState = &multisampleStateCI;
    pipelineCI.pViewportState = &viewportStateCI;
    pipelineCI.pDepthStencilState = &depthStencilStateCI;
    pipelineCI.pDynamicState = &dynamicStateCI;
    pipelineCI.stageCount = static_cast<uint32_t>(mRenderable->mShaderStages.size());
    pipelineCI.pStages = mRenderable->mShaderStages.data();

    mPiplines.resize(1);
    VK_CHECK_RESULT(vkCreateGraphicsPipelines(GGVulkanSingleHandle::getVulkanDevice().logicalDevice, mPipelineCache, 1,
                                              &pipelineCI, nullptr, &mPiplines[0]));
}

}  // namespace Gange