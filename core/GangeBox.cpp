#include "GangeBox.h"
#include "GGVulkanTools.h"

namespace Gange {
ModelBox::ModelBox(const Vector3 &halfSize) {}

ModelBox::ModelBox(bool protagonist)
    : Renderable() {
    mProtagonist = protagonist;
    if (mProtagonist) {
        initialize();
    } else {
        initializeSkyBox();
    }
}

ModelBox::~ModelBox(){};

void ModelBox::initialize() {
    float gSizeX = 10.0f;
	float posY = 1.0f;
    float negY = 21.0f;
    float gSizeZ = 10.0f;
    float rept = 1.0f;

    std::vector<Vector3> position = {
        {-gSizeX, posY, gSizeZ},      {gSizeX, posY, gSizeZ},      {gSizeX, posY, -gSizeZ},      {-gSizeX, posY, -gSizeZ},

        {-gSizeX, negY, gSizeZ}, {gSizeX, negY, gSizeZ}, {gSizeX, negY, -gSizeZ}, {-gSizeX, negY, -gSizeZ},
    };

    Vector4 color(1.0f);
    std::vector<Vector3> normal = {
        {1.0f, 0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, -1.0f},
    };

    std::vector<Vector2> uv{{0.0f, 0.0f}, {0.0f, rept}, {rept, rept}, {rept, 0.0f}};
    mVextices = {
        // button
        {position[2], normal[3], color, uv[0]},
        {position[1], normal[3], color, uv[1]},
        {position[0], normal[3], color, uv[2]},
        {position[3], normal[3], color, uv[3]},

        // right
        {position[0], normal[1], color, uv[0]},
        {position[4], normal[1], color, uv[1]},
        {position[7], normal[1], color, uv[2]},
        {position[3], normal[1], color, uv[3]},

        // back
        {position[7], normal[5], color, uv[0]},
        {position[6], normal[5], color, uv[1]},
        {position[2], normal[5], color, uv[2]},
        {position[3], normal[5], color, uv[3]},

        // front
        {position[4], normal[4], color, uv[0]},
        {position[0], normal[4], color, uv[1]},
        {position[1], normal[4], color, uv[2]},
        {position[5], normal[4], color, uv[3]},

        // left
        {position[5], normal[0], color, uv[0]},
        {position[1], normal[0], color, uv[1]},
        {position[2], normal[0], color, uv[2]},
        {position[6], normal[0], color, uv[3]},

        // top
        {position[4], normal[2], color, uv[0]},
        {position[5], normal[2], color, uv[1]},
        {position[6], normal[2], color, uv[2]},
        {position[7], normal[2], color, uv[3]},
    };

    mIndices = {0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

    loadVao(mVextices, mIndices);

    std::string imagePath =  getAssetPath() + "textures/lenna.jpg";
    loadTexture(imagePath.c_str());

	mModelMat.setTrans(Vector3(100,0.0,0.0));
}

void ModelBox::initializeSkyBox() {
    float gSizeX = 100.0f;
    float negY = 100.0f;
    float gSizeZ = 100.0f;
    float rept = 1.0f;

    std::vector<Vector3> position = {
        {-gSizeX, -negY, gSizeZ}, {gSizeX, -negY, gSizeZ}, {gSizeX, -negY, -gSizeZ}, {-gSizeX, -negY, -gSizeZ},

        {-gSizeX, negY, gSizeZ},  {gSizeX, negY, gSizeZ},  {gSizeX, negY, -gSizeZ},  {-gSizeX, negY, -gSizeZ},
    };

    std::vector<Vector3> normal = {
        {1.0f, 0.0f, 0.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, -1.0f},
    };
    Vector4 color(1.0f);
    std::vector<Vector2> uv{{0.0f, 0.0f}, {0.0f, rept}, {rept, rept}, {rept, 0.0f}};
    mVextices = {
        // button
        {position[1], normal[3], color, uv[0]},
        {position[2], normal[3], color, uv[1]},
        {position[3], normal[3], color, uv[2]},
        {position[0], normal[3], color, uv[3]},

        // right
        {position[4], normal[1], color, uv[0]},
        {position[0], normal[1], color, uv[1]},
        {position[3], normal[1], color, uv[2]},
        {position[7], normal[1], color, uv[3]},

        // back
        {position[7], normal[5], color, uv[0]},
        {position[3], normal[5], color, uv[1]},
        {position[2], normal[5], color, uv[2]},
        {position[6], normal[5], color, uv[3]},

        // top
        {position[5], normal[2], color, uv[0]},
        {position[4], normal[2], color, uv[1]},
        {position[7], normal[2], color, uv[2]},
        {position[6], normal[2], color, uv[3]},

        // left
        {position[6], normal[0], color, uv[0]},
        {position[2], normal[0], color, uv[1]},
        {position[1], normal[0], color, uv[2]},
        {position[5], normal[0], color, uv[3]},

        // front
        {position[5], normal[4], color, uv[0]},
        {position[1], normal[4], color, uv[1]},
        {position[0], normal[4], color, uv[2]},
        {position[4], normal[4], color, uv[3]},
    };

    mIndices = {0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
                12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

    loadVao(mVextices, mIndices);

    std::vector<std::string> imagesPath;
    std::string imagePath = getAssetPath() + "textures/cube/cube_negy.png";
    imagesPath.push_back(imagePath);
    imagePath = getAssetPath() + "textures/cube/cube_posx.png";
    imagesPath.push_back(imagePath);
    imagePath = getAssetPath() + "textures/cube/cube_negz.png";
    imagesPath.push_back(imagePath);
    imagePath = getAssetPath() + "textures/cube/cube_posy.png";
    imagesPath.push_back(imagePath);
    imagePath = getAssetPath() + "textures/cube/cube_negx.png";
    imagesPath.push_back(imagePath);
    imagePath = getAssetPath() + "textures/cube/cube_posz.png";
    imagesPath.push_back(imagePath);
    loadTexture(imagesPath);
    flag = true;
}
}  // namespace Gange