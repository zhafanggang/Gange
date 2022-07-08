#include "GGVertexBufferHandle.h"
#include "GGVulkanGlTFModel.h"
#include <vector>

namespace Gange {

GGVertexBufferHandle::GGVertexBufferHandle() {}

GGVertexBufferHandle::~GGVertexBufferHandle() {}

void GGVertexBufferHandle::generate() {}

void GGVertexBufferHandle::generate(std::vector<Vertex> &vertices) {
    mVerticesCount = (uint32_t) vertices.size();
    VK_CHECK_RESULT(mVulkanDevice->createBuffer(
        mVerticesCount * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mBuffer, vertices.data()));
}

}  // namespace Gange
