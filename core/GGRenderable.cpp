#include "GGRenderable.h"
#include "GGVulkanTools.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {
Renderable::Renderable() {
    mVulkanDevice = GGVulkanSingleHandle::getVulkanDevicePtr();
    mIndexBuffer = new GGIndexBufferHandle();
    mVertexBuffer = new GGVertexBufferHandle();
    mUniformbuffer = new GGUniformBufferHandle();
}

Renderable::~Renderable() {
    delete mIndexBuffer;
    delete mVertexBuffer;
    delete mUniformbuffer;
}

void Renderable::update() {
    mUniformbuffer->updateUniformBuffers();
}

void Renderable::setCameraController(GGCameraController *cameraController) {
    mUniformbuffer->setCameraController(cameraController);
}

void Renderable::initialize() {
    mTextureImage = new GGTextureImage(mVulkanDevice, GGVulkanSingleHandle::getVkQueue());

    mVertexBuffer->generate();
    mIndexBuffer->generate();
    mUniformbuffer->generate();
    mTextureImage->create();

    mShaderStages.resize(2);
    mShaderStages[0];

    mShaderStages[0] = loadShader("../Data/shaders/single/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    mShaderStages[1] = loadShader("../Data/shaders/single/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    setupDescriptorSetLayout();
    setupDescriptorSet();
}

void Renderable::setupDescriptorSetLayout() {

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        // Binding 0 : Vertex shader uniform buffer
        initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
        // Binding 1 : Fragment shader image sampler
        initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                 VK_SHADER_STAGE_FRAGMENT_BIT, 1)};

    VkDescriptorSetLayoutCreateInfo descriptorLayout = initializers::descriptorSetLayoutCreateInfo(
        setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));
    mDescriptorSetLayouts.resize(1);
    vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorLayout, nullptr, &mDescriptorSetLayouts[0]);

    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
        initializers::pipelineLayoutCreateInfo(&mDescriptorSetLayouts[0], 1);

    vkCreatePipelineLayout(mVulkanDevice->logicalDevice, &pPipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
}

void Renderable::setupDescriptorSet() {

    std::vector<VkDescriptorPoolSize> poolSizes = {
        initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
        initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)};

    VkDescriptorPoolCreateInfo descriptorPoolInfo =
        initializers::descriptorPoolCreateInfo(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(), 2);

    VK_CHECK_RESULT(
        vkCreateDescriptorPool(mVulkanDevice->logicalDevice, &descriptorPoolInfo, nullptr, &mDescriptorPool));

    VkDescriptorSetAllocateInfo allocInfo =
        initializers::descriptorSetAllocateInfo(mDescriptorPool, &mDescriptorSetLayouts[0], 1);
    VK_CHECK_RESULT(
        vkAllocateDescriptorSets(mVulkanDevice->logicalDevice, &allocInfo, &mUniformbuffer->mDescriptorSet));
    mTextureImage->mDescriptorSet = mUniformbuffer->mDescriptorSet;

    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
        // Binding 0 : Vertex shader uniform buffer
        initializers::writeDescriptorSet(mUniformbuffer->mDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
                                         &mUniformbuffer->getBuffer()->descriptor),
        // Binding 1 : Fragment shader texture sampler
        //	Fragment shader: layout (binding = 1) uniform sampler2D samplerColor;
        initializers::writeDescriptorSet(
            mTextureImage->mDescriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
            &mTextureImage->mDescriptorImageInfo)  // Pointer to the descriptor image for our texture
    };
    vkUpdateDescriptorSets(mVulkanDevice->logicalDevice, 2, writeDescriptorSets.data(), 0, nullptr);
}

VkPipelineShaderStageCreateInfo Renderable::loadShader(std::string fileName, VkShaderStageFlagBits stage) {
    VkPipelineShaderStageCreateInfo shaderStage = {};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = stage;
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
    shaderStage.module = tools::loadShader(androidApp->activity->assetManager, fileName.c_str(), device);
#else
    shaderStage.module = Vulkantools::loadShader(fileName.c_str(), mVulkanDevice->logicalDevice);
#endif
    shaderStage.pName = "main";
    assert(shaderStage.module != VK_NULL_HANDLE);
    mShaderModules.push_back(shaderStage.module);
    return shaderStage;
}

}  // namespace Gange