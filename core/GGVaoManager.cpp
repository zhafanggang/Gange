#include "GGVaoManager.h"
#include "GangeBox.h"
#include "GangePlane.h"
#include "GGVulkanGlTFModel.h"

namespace Gange {

VaoManager::VaoManager() {
    setupVertexInputState();
}

VaoManager::~VaoManager() {
    for (auto iter : mRenderVec) {
        delete iter;
    }
}

void VaoManager::setUniform(std::vector<GGUniformBufferHandle *> uniformBufferVec) {
	
		Plane *plane = new Plane();
		plane->setuniformBuffers(uniformBufferVec);
		mRenderVec.push_back(plane);

		GGVulkanGlTFModel *model = new GGVulkanGlTFModel(true);
		model->setuniformBuffers(uniformBufferVec);
		mRenderVec.push_back(model);

}

void VaoManager::buildCommandBuffers(VkCommandBuffer commandBuffer) {
    for (auto iter : mRenderVec) {
        iter->buildCommandBuffers(commandBuffer);
    }
}

const Renderable &VaoManager::createRanderable() {
    Renderable *randerable = new Renderable();
    return *randerable;
}

void VaoManager::setDlout(VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout) {
    mDescriptorSet = descriptorSet;
    mPipelineLayout = pipelineLayout;
}

VkPipelineVertexInputStateCreateInfo VaoManager::getVertexInputState() {
    return mVertexInputState.inputState;
}

void VaoManager::setupVertexInputState() {
    // Binding description
    mVertexInputState.bindingDescriptions.resize(1);
    mVertexInputState.bindingDescriptions[0] =
        initializers::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);

    mVertexInputState.attributeDescriptions.resize(4);
    // Location 0 : Position
    mVertexInputState.attributeDescriptions[0] =
        initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
    mVertexInputState.attributeDescriptions[1] =
        initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
    // Location 1 : Texture coordinates
	mVertexInputState.attributeDescriptions[2] =
		initializers::vertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, color));
    mVertexInputState.attributeDescriptions[3] =
        initializers::vertexInputAttributeDescription(0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv));

    mVertexInputState.inputState = initializers::pipelineVertexInputStateCreateInfo();
    mVertexInputState.inputState.vertexBindingDescriptionCount =
        static_cast<uint32_t>(mVertexInputState.bindingDescriptions.size());
    mVertexInputState.inputState.pVertexBindingDescriptions = mVertexInputState.bindingDescriptions.data();
    mVertexInputState.inputState.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(mVertexInputState.attributeDescriptions.size());
    mVertexInputState.inputState.pVertexAttributeDescriptions = mVertexInputState.attributeDescriptions.data();
}

}  // namespace Gange
