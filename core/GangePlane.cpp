#include "GangePlane.h"

namespace Gange {
void Plane::initialize() {
    Renderable::initialize();
}

void Plane::draw(VkCommandBuffer commandBuffer) {
    // All vertices and indices are stored in single buffers, so we only need to bind once
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mVertexBuffer->getBuffer()->buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer->getBuffer()->buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                            &mTextureImage->mDescriptorSet, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
}
}  // namespace Gange
