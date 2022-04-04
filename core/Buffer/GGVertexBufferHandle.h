#ifndef _GG_VERTEX_BUFFER_HANDLE_
#define _GG_VERTEX_BUFFER_HANDLE_

#include "GGBufferHandle.h"
#include "GGVulkanBuffer.h"
#include "Math/GGVector4.h"

namespace Gange {

struct Vertex {
    Vector3 pos;
    Vector3 normal;
	Vector4 color;
    Vector2 uv;
};

class GGVertexBufferHandle : public GGBufferHandle {

public:
    GGVertexBufferHandle();

    virtual ~GGVertexBufferHandle();

    virtual void generate() override;

    virtual void generate(std::vector<Vertex> &vertices);

    uint32_t mVerticesCount;
};

}  // namespace Gange

#endif