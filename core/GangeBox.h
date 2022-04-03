#ifndef _GANGE_BOX_H_
#define _GANGE_BOX_H_

#include "Math/GGVector3.h"
#include <vector>
#include "GGRenderable.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {
class ModelBox : public Renderable {
public:
    struct V3N3 {
        Vector3 position;
        Vector3 normal;
        Vector2 uv;
    };

    V3N3 _data[24];

public:
    ModelBox(const Vector3 &halfSize) {
        float gSizeX = 50.0f;
        float gSizeY = 50.0f;
        float gSizeZ = 50.0f;
        float rept = 1;

        std::vector<Vector3> position = {
            {-gSizeX, gSizeY, gSizeZ},  {gSizeX, gSizeY, gSizeZ},
            {gSizeX, gSizeY, -gSizeZ},  {-gSizeX, gSizeY, -gSizeZ},

            {-gSizeX, -gSizeY, gSizeZ}, {gSizeX, -gSizeY, gSizeZ},
            {gSizeX, -gSizeY, -gSizeZ}, {-gSizeX, -gSizeY, -gSizeZ},
        };

        std::vector<Vector3> normal = {
            {1.0f, 0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
            {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, -1.0f},
        };

        std::vector<Vector2> uv{{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}};

        V3N3 verts[] = {
            {position[0], normal[3], uv[0]}, {position[3], normal[3], uv[1]},
            {position[2], normal[3], uv[2]}, {position[1], normal[3], uv[3]},

            {position[4], normal[4], uv[0]}, {position[0], normal[4], uv[1]},
            {position[1], normal[4], uv[2]}, {position[5], normal[4], uv[3]},

            {position[5], normal[0], uv[0]}, {position[1], normal[0], uv[1]},
            {position[2], normal[0], uv[2]}, {position[6], normal[0], uv[3]},

            {position[4], normal[2], uv[0]}, {position[5], normal[2], uv[1]},
            {position[6], normal[2], uv[2]}, {position[7], normal[2], uv[3]},

            {position[6], normal[5], uv[0]}, {position[2], normal[5], uv[1]},
            {position[3], normal[5], uv[2]}, {position[7], normal[5], uv[3]},

            {position[7], normal[1], uv[0]}, {position[3], normal[1], uv[1]},
            {position[0], normal[1], uv[2]}, {position[4], normal[1], uv[3]},
        };
        memcpy(_data, verts, sizeof(verts));
    }

    void initialize() {

        mTextureImage = new GGTextureImage(mVulkanDevice, GGVulkanSingleHandle::getVkQueue());
    }
};
}  // namespace Gange

#endif
