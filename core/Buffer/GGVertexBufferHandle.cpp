#include "GGVertexBufferHandle.h"
#include "GGVulkanGlTFModel.h"
#include <vector>

namespace Gange {

GGVertexBufferHandle::GGVertexBufferHandle() {}

GGVertexBufferHandle::~GGVertexBufferHandle() {}

void GGVertexBufferHandle::generate() {
    float gSizeX = 100.0f;
    float gSizeY = 0.0f;
    float gSizeZ = 50.0f;
    float rept = 1;

    /*std::vector<Vertex> vertices = {
    {{-gSizeX, gSizeY, gSizeZ}, {0.0f,rept}},
    {{gSizeX, gSizeY, gSizeZ}, {rept, rept}},
    {{gSizeX, gSizeY, -gSizeZ}, {rept, 0.0f}},
    {{-gSizeX, gSizeY, -gSizeZ}, {0.0f,0.0f}}, };*/

    std::vector<Vertex> vertices = {{{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f}},
                                    {{0.5f, 0.0f, -0.5f}, {0.0f, 0.0f}},
                                    {{0.5f, 0.0f, 0.5f}, {0.0f, 1.0f}},
                                    {{-0.5f, 0.0f, 0.5f}, {1.0f, 1.0f}}};

    VK_CHECK_RESULT(mVulkanDevice->createBuffer(
        vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mBuffer, vertices.data()));

    {
        // Binding description
        mBindingDescriptions.resize(1);
        mBindingDescriptions[0] =
            initializers::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);

        mAttributeDescriptions.resize(2);
        // Location 0 : Position
        mAttributeDescriptions[0] =
            initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
        // Location 1 : Texture coordinates
        mAttributeDescriptions[1] =
            initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv));

        mInputState = initializers::pipelineVertexInputStateCreateInfo();
        mInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindingDescriptions.size());
        mInputState.pVertexBindingDescriptions = mBindingDescriptions.data();
        mInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(mAttributeDescriptions.size());
        mInputState.pVertexAttributeDescriptions = mAttributeDescriptions.data();
    }
}

void GGVertexBufferHandle::create() {
    mBindingDescriptions = {
        initializers::vertexInputBindingDescription(0, sizeof(GGVulkanGlTFModel::Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
    };

    mAttributeDescriptions.resize(3);
    // Location 0 : Position
    mAttributeDescriptions[0] = initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT,
                                                                              offsetof(GGVulkanGlTFModel::Vertex, pos));
    // Location 1 : Texture coordinates
    mAttributeDescriptions[1] = initializers::vertexInputAttributeDescription(
        0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(GGVulkanGlTFModel::Vertex, normal));

    mAttributeDescriptions[2] = initializers::vertexInputAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT,
                                                                              offsetof(GGVulkanGlTFModel::Vertex, uv));

    mInputState = initializers::pipelineVertexInputStateCreateInfo();
    mInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(mBindingDescriptions.size());
    mInputState.pVertexBindingDescriptions = mBindingDescriptions.data();
    mInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(mAttributeDescriptions.size());
    mInputState.pVertexAttributeDescriptions = mAttributeDescriptions.data();
}
}  // namespace Gange
