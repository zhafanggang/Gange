#include "GGRenderable.h"
#include "GGVulkanTools.h"
#include "GGVulkanSingleHandle.h"
#include "buffer/GGTextureImage.h"

namespace Gange {
Renderable::Renderable() {
    mVulkanDevice = VulkanSingleHandle::getVulkanDevicePtr();
}

Renderable::~Renderable() {
    for (auto &iter : mImages) {
        delete iter;
    }
    mImages.clear();

    vkDestroyBuffer(mVulkanDevice->logicalDevice, mVertexBuffer, nullptr);
    vkFreeMemory(mVulkanDevice->logicalDevice, mVertexMemory, nullptr);
    vkDestroyBuffer(mVulkanDevice->logicalDevice, mIndexBuffer, nullptr);
    vkFreeMemory(mVulkanDevice->logicalDevice, mIndexMemory, nullptr);
}

void Renderable::loadVao(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
    mIndexCount = static_cast<uint32_t>(indices.size());
    mVertexCount = static_cast<uint32_t>(vertices.size());

    size_t vertexBufferSize = mVertexCount * sizeof(Vertex);
    size_t indexBufferSize = mIndexCount * sizeof(uint32_t);

    struct StagingBuffer {
        VkBuffer buffer;
        VkDeviceMemory memory;
    } vertexStaging, indexStaging;

    // Create host visible staging buffers (source)
    VK_CHECK_RESULT(
        mVulkanDevice->createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &vertexStaging.buffer, &vertexStaging.memory, vertices.data()));
    // Index data
    VK_CHECK_RESULT(
        mVulkanDevice->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &indexStaging.buffer, &indexStaging.memory, indices.data()));

    // Create device local buffers (target)
    VK_CHECK_RESULT(mVulkanDevice->createBuffer(vertexBufferSize,
                                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mVertexBuffer, &mVertexMemory));
    VK_CHECK_RESULT(mVulkanDevice->createBuffer(indexBufferSize,
                                                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mIndexBuffer, &mIndexMemory));

    VkCommandBuffer copyCmd = mVulkanDevice->beginOnceCommand();
    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, vertexStaging.buffer, mVertexBuffer, 1, &copyRegion);
    copyRegion.size = indexBufferSize;
    vkCmdCopyBuffer(copyCmd, indexStaging.buffer, mIndexBuffer, 1, &copyRegion);
    mVulkanDevice->endOnceCommand(copyCmd);

    vkDestroyBuffer(mVulkanDevice->logicalDevice, vertexStaging.buffer, nullptr);
    vkFreeMemory(mVulkanDevice->logicalDevice, vertexStaging.memory, nullptr);
    vkDestroyBuffer(mVulkanDevice->logicalDevice, indexStaging.buffer, nullptr);
    vkFreeMemory(mVulkanDevice->logicalDevice, indexStaging.memory, nullptr);
}

void Renderable::buildCommandBuffers(VkCommandBuffer commandBuffer) {
    vkCmdPushConstants(commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Matrix4), &mModelMat);
    if (mProtagonist) {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                &mUniformbuffers[0]->mDescriptorSet, 0, nullptr);
    } else {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                &mUniformbuffers[1]->mDescriptorSet, 0, nullptr);
    }
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 2, 1,
		&mUniformbuffers[2]->mDescriptorSet, 0, nullptr);
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mVertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

    if (flag) {
        for (uint32_t i = 0, j = 0; i < mIndexCount; j++, i += 6) {
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 1, 1,
                                    &mImages[j]->mDescriptorSet, 0, nullptr);
            vkCmdDrawIndexed(commandBuffer, 6, 1, i, 0, 0);
        }
    } else {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 1, 1,
                                &mImages[0]->mDescriptorSet, 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, mIndexCount, 1, 0, 0, 0);
    }
}

void Renderable::loadTexture(const std::vector<const char *> &fileName) {
    for (auto &imagePath : fileName) {
        loadTexture(imagePath);
    }
}

void Renderable::loadTexture(const char *fileName) {
    GGTextureImage *texture = new GGTextureImage();
    texture->loadFromFile(fileName);
    mImages.push_back(texture);
}

void Renderable::setuniformBuffers(std::vector<GGUniformBufferHandle *> uniformbuffers) {
    mUniformbuffers = uniformbuffers;

    setupDescriptorSet();
}

void Renderable::setupDescriptorSet() {
    uint32_t imageCount = (uint32_t) mImages.size();
	std::vector<VkDescriptorPoolSize> poolSizes = {
		initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3)
	};
	
	if (imageCount > 0)
	{
		poolSizes.push_back(initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageCount)) ;
	}

    VkDescriptorPoolCreateInfo descriptorPoolInfo =
        initializers::descriptorPoolCreateInfo((uint32_t) poolSizes.size(), poolSizes.data(), imageCount + 3);
    VK_CHECK_RESULT(
        vkCreateDescriptorPool(mVulkanDevice->logicalDevice, &descriptorPoolInfo, nullptr, &mDescriptorPool));

    // Descriptor set layout for passing matrices
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0)};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI = initializers::descriptorSetLayoutCreateInfo(
        setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorSetLayoutCI, nullptr,
                                                &mDescriptorSetLayoutUbo));

    // Descriptor set layout for passing matrices
    setLayoutBindings = {initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
																  VK_SHADER_STAGE_FRAGMENT_BIT, 0),};
    descriptorSetLayoutCI = initializers::descriptorSetLayoutCreateInfo(
        setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorSetLayoutCI, nullptr,
                                                &mDescriptorSetLayoutImage));

	// Descriptor set layout for passing matrices
	setLayoutBindings = { initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
																  VK_SHADER_STAGE_FRAGMENT_BIT, 0), };
	descriptorSetLayoutCI = initializers::descriptorSetLayoutCreateInfo(
		setLayoutBindings.data(), static_cast<uint32_t>(setLayoutBindings.size()));

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorSetLayoutCI, nullptr,
		&mDescriptorSetLayoutFragUbo));

    // Pipeline layout using both descriptor sets (set 0 = matrices, set 1 = material)
    std::array<VkDescriptorSetLayout, 3> setLayouts = {mDescriptorSetLayoutUbo, mDescriptorSetLayoutImage, mDescriptorSetLayoutFragUbo};
    VkPipelineLayoutCreateInfo pipelineLayoutCI =
        initializers::pipelineLayoutCreateInfo(setLayouts.data(), static_cast<uint32_t>(setLayouts.size()));

    VkPushConstantRange pushConstantRange =
        initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(Matrix4), 0);
    // Push constant ranges are part of the pipeline layout
    pipelineLayoutCI.pushConstantRangeCount = 1;
    pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

    VK_CHECK_RESULT(vkCreatePipelineLayout(mVulkanDevice->logicalDevice, &pipelineLayoutCI, nullptr, &mPipelineLayout));

	for (int i = 0; i < 2;i++ ) {
        VkDescriptorSetAllocateInfo allocInfo =
            initializers::descriptorSetAllocateInfo(mDescriptorPool, &mDescriptorSetLayoutUbo, 1);
        VK_CHECK_RESULT(vkAllocateDescriptorSets(mVulkanDevice->logicalDevice, &allocInfo, &mUniformbuffers[i]->mDescriptorSet));
		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			// Binding 0 : Vertex shader uniform buffer
			initializers::writeDescriptorSet(mUniformbuffers[i]->mDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
											 &mUniformbuffers[i]->getBuffer()->descriptor), };
        vkUpdateDescriptorSets(mVulkanDevice->logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(), 0, NULL);
    }

    for (auto &iter : mImages) {
        VkDescriptorSetAllocateInfo allocInfo =
            initializers::descriptorSetAllocateInfo(mDescriptorPool, &mDescriptorSetLayoutImage, 1);
        VK_CHECK_RESULT(vkAllocateDescriptorSets(mVulkanDevice->logicalDevice, &allocInfo, &iter->mDescriptorSet));

        std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
            // Binding 0 : Vertex shader uniform buffer
            initializers::writeDescriptorSet(iter->mDescriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                                             &iter->mDescriptorImageInfo) ,
		initializers::writeDescriptorSet(iter->mDescriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
											 &iter->mDescriptorImageInfo) };
        vkUpdateDescriptorSets(mVulkanDevice->logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(), 0, NULL);
    }

	{
		VkDescriptorSetAllocateInfo allocInfo =
			initializers::descriptorSetAllocateInfo(mDescriptorPool, &mDescriptorSetLayoutFragUbo, 1);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(mVulkanDevice->logicalDevice, &allocInfo, &mUniformbuffers[2]->mDescriptorSet));
		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			// Binding 0 : Vertex shader uniform buffer
			initializers::writeDescriptorSet(mUniformbuffers[2]->mDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
											 &mUniformbuffers[2]->getBuffer()->descriptor), };
		vkUpdateDescriptorSets(mVulkanDevice->logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()),
			writeDescriptorSets.data(), 0, NULL);
	}
	

}

}  // namespace Gange