#include "GGRenderPipline.h"
#include "GGVulkanInitializers.h"
#include "GGVulkanSingleHandle.h"
#include "GGVulkanGlTFModel.h"
#include "GGVulkanTools.h"

namespace Gange {
GGRenderPipline::GGRenderPipline(VkRenderPass renderPass, VkSampleCountFlagBits sampleCount) {
    mRenderPass = renderPass;
    mSampleCount = sampleCount;

    mUniformBufferManger = new UniformManager();
    
    mVaoManager = new VaoManager();

    mVaoManager->setUniform(mUniformBufferManger->mUniformBufferVec);

    mShaderManager = new ShaderManager();

    uniformBuffer = new GGUniformBufferHandle();

    uniformBuffer->generate();
}

GGRenderPipline::~GGRenderPipline() {
    delete mShaderManager;
    delete mVaoManager;
    delete mUniformBufferManger;
}

void GGRenderPipline::initialize() {
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VK_CHECK_RESULT(vkCreatePipelineCache(VulkanSingleHandle::getVulkanDevice().logicalDevice, &pipelineCacheCreateInfo,
                                          nullptr, &mPipelineCache));

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI =
        initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
    VkPipelineRasterizationStateCreateInfo rasterizationStateCI = initializers::pipelineRasterizationStateCreateInfo(
        VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
    VkPipelineColorBlendAttachmentState blendAttachmentStateCI =
        initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
    VkPipelineColorBlendStateCreateInfo colorBlendStateCI =
        initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentStateCI);
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCI =
        initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS);
    VkPipelineViewportStateCreateInfo viewportStateCI = initializers::pipelineViewportStateCreateInfo(1, 1, 0);
    VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
    multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // Number of samples to use for rasterization
    multisampleStateCI.rasterizationSamples = mSampleCount;
    const std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateCI = initializers::pipelineDynamicStateCreateInfo(
        dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);

    VkGraphicsPipelineCreateInfo pipelineCI =
        initializers::pipelineCreateInfo(mVaoManager->mRenderVec[0]->mPipelineLayout, mRenderPass, 0);
     //TODO:
    VkPipelineVertexInputStateCreateInfo inputState = mVaoManager->getVertexInputState();
    pipelineCI.pVertexInputState = &inputState;
    pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
    pipelineCI.pRasterizationState = &rasterizationStateCI;
    pipelineCI.pColorBlendState = &colorBlendStateCI;
    pipelineCI.pMultisampleState = &multisampleStateCI;
    pipelineCI.pViewportState = &viewportStateCI;
    pipelineCI.pDepthStencilState = &depthStencilStateCI;
    pipelineCI.pDynamicState = &dynamicStateCI;
    pipelineCI.stageCount = static_cast<uint32_t>(mShaderManager->getShaderStages().size());
    pipelineCI.pStages = mShaderManager->getShaderStages().data();

    mPiplines.resize(2);
    VK_CHECK_RESULT(vkCreateGraphicsPipelines(VulkanSingleHandle::getVulkanDevice().logicalDevice, mPipelineCache, 1,
                                              &pipelineCI, nullptr, &mPiplines[0]));

    rasterizationStateCI = initializers::pipelineRasterizationStateCreateInfo(
        VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
    pipelineCI.pRasterizationState = &rasterizationStateCI;
    VK_CHECK_RESULT(vkCreateGraphicsPipelines(VulkanSingleHandle::getVulkanDevice().logicalDevice, mPipelineCache, 1,
                                              &pipelineCI, nullptr, &mPiplines[1]));
}

void GGRenderPipline::buildCommandBuffers(VkCommandBuffer commandBuffer) {
    int i = 0;
    for (auto &pipline : mPiplines) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipline);
        mVaoManager->buildCommandBuffers(commandBuffer);
    }
}

void GGRenderPipline::update() {
    mUniformBufferManger->update();
}

}  // namespace Gange