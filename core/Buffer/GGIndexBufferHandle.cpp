#include "GGIndexBufferHandle.h"

namespace Gange {
GGIndexBufferHandle::GGIndexBufferHandle() {}

GGIndexBufferHandle::~GGIndexBufferHandle() {}

void GGIndexBufferHandle::generate() {
    std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
    mIndexCount = static_cast<uint32_t>(indices.size());

    VK_CHECK_RESULT(mVulkanDevice->createBuffer(
        indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mBuffer, indices.data()));
}

}  // namespace Gange
