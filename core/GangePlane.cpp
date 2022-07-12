#include "GangePlane.h"
#include "GGVulkanTools.h"

namespace Gange {
void Plane::initialize() {
    float gSizeX = 2000.0f;
    float gSizeY = -5.0f;
    float gSizeZ = 1000.0f;
    float rept = 20;

	Vector4 color(1.0f);

    mVextices = {
        {{-gSizeX, gSizeY, gSizeZ}, {0.0f, 1.0f, 0.0f}, color,{0.0f, rept}},
		{{gSizeX, gSizeY, gSizeZ}, {0.0f, 1.0f, 0.0f}, color,{rept, rept}},
		{{gSizeX, gSizeY, -gSizeZ}, {0.0f, 1.0f, 0.0f}, color,{rept, 0.0f}},
        {{-gSizeX, gSizeY, -gSizeZ}, {0.0f, 1.0f, 0.0f}, color,{0.0f, 0.0f}},
    };

    mIndices = {0, 1, 2, 2, 3, 0};

    loadVao(mVextices, mIndices);
    std::string imagePath =  getAssetPath() + "textures/cube.png";

    loadTexture(imagePath.c_str());
}

}  // namespace Gange
