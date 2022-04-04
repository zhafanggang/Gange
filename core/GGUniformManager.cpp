#include "GGUniformManager.h"
#include "GGVulkanSingleHandle.h"
#include "GGVulkanInitializers.h"

namespace Gange {

UniformManager::UniformManager() {
    mVulkanDevice = VulkanSingleHandle::getVulkanDevicePtr();
    mQueueVec.push_back(VulkanSingleHandle::getVkQueue());
    initialize();
}

UniformManager::~UniformManager() {
    vkDestroyPipelineLayout(mVulkanDevice->logicalDevice, mPipelineLayout, nullptr);
}

void UniformManager::initialize() {
    GGUniformBufferHandle *uniformBufferSkyBox = new GGUniformBufferHandle(UniformType::scene,true);

    uniformBufferSkyBox->generate();

    mUniformBufferVec.push_back(uniformBufferSkyBox);

    GGUniformBufferHandle *uniformBuffer = new GGUniformBufferHandle();

    uniformBuffer->generate();

    mUniformBufferVec.push_back(uniformBuffer);

	GGUniformBufferHandle *uniformBuffer1 = new GGUniformBufferHandle(UniformType::light);

	uniformBuffer1->generate();

	mUniformBufferVec.push_back(uniformBuffer1);
}

void UniformManager::setupDescriptorSetLayout() {

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {

        initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),

        initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                 VK_SHADER_STAGE_FRAGMENT_BIT, 0),
	initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
												 VK_SHADER_STAGE_FRAGMENT_BIT, 1) };

    VkDescriptorSetLayoutCreateInfo descriptorLayout = initializers::descriptorSetLayoutCreateInfo(
        setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));
    if (1) {
        mDescriptorSetLayouts.resize(3);
        for (auto &iter : mDescriptorSetLayouts) {
            vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorLayout, nullptr, &iter);
        }
    } else {
        mDescriptorSetLayouts.resize(1);
        vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorLayout, nullptr,
                                    &mDescriptorSetLayouts[0]);
    }

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = initializers::pipelineLayoutCreateInfo(
        mDescriptorSetLayouts.data(), static_cast<uint32_t>(mDescriptorSetLayouts.size()));

    if (mUniformBufferVec[0]->addPushConstant) {
        VkPushConstantRange pushConstantRange =
            initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(uint32_t), 0);
        pPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pPipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    }
    vkCreatePipelineLayout(mVulkanDevice->logicalDevice, &pPipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
}

void UniformManager::setupDescriptorSet() {
    uint32_t textureCount = (uint32_t) mTextureVec.size();
    std::vector<VkDescriptorPoolSize> poolSizes = {
        initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
        initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, textureCount),};

    VkDescriptorPoolCreateInfo descriptorPoolInfo = initializers::descriptorPoolCreateInfo(
        static_cast<uint32_t>(poolSizes.size()), poolSizes.data(), textureCount + 2);

    VK_CHECK_RESULT(
        vkCreateDescriptorPool(mVulkanDevice->logicalDevice, &descriptorPoolInfo, nullptr, &mDescriptorPool));

    if (mDescriptorSetLayouts.size() == 1) {
        VkDescriptorSetAllocateInfo allocInfo =
            initializers::descriptorSetAllocateInfo(mDescriptorPool, &mDescriptorSetLayouts[0], 1);
        VK_CHECK_RESULT(
            vkAllocateDescriptorSets(mVulkanDevice->logicalDevice, &allocInfo, &mUniformBufferVec[0]->mDescriptorSet));
        mTextureVec[0]->mDescriptorSet = mUniformBufferVec[0]->mDescriptorSet;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
            // Binding 0 : Vertex shader uniform buffer
            initializers::writeDescriptorSet(mUniformBufferVec[0]->mDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                             &mUniformBufferVec[0]->getBuffer()->descriptor),
            // Binding 1 : Fragment shader texture sampler
            //	Fragment shader: layout (binding = 1) uniform sampler2D samplerColor;
            initializers::writeDescriptorSet(mTextureVec[0]->mDescriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                             1, &mTextureVec[0]->mDescriptorImageInfo)};
        vkUpdateDescriptorSets(mVulkanDevice->logicalDevice, 2, writeDescriptorSets.data(), 0, nullptr);
    } else {
    }
}

void UniformManager::update() {
    for (const auto &uniformBuffer : mUniformBufferVec) {
        uniformBuffer->updateUniformBuffers();
    }
}

}  // namespace Gange
