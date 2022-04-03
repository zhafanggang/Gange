#ifndef _GG_VERTEX_BUFFER_HANDLE_
#define _GG_VERTEX_BUFFER_HANDLE_

#include "GGBufferHandle.h"
#include "GGVulkanBuffer.h"
#include "Math/GGVector3.h"

namespace Gange {

struct Vertex {
    Vector3 pos;
    Vector2 uv;
};

class GGVertexBufferHandle : public GGBufferHandle {

public:
    GGVertexBufferHandle();

    virtual ~GGVertexBufferHandle();

    virtual void generate() override;

    void create();

    VkPipelineVertexInputStateCreateInfo mInputState;

    std::vector<VkVertexInputBindingDescription> mBindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions;
};

}  // namespace Gange

#endif