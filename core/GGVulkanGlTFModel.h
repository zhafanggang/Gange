#ifndef _GG_VULKAN_GLTF_MODEL_H_
#define _GG_VULKAN_GLTF_MODEL_H_

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <limits>

#include "vulkan/vulkan.h"
#include "GGVulkanDevice.h"

#include "buffer/GGTextureImage.h"
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif
#include "tinygltf/tiny_gltf.h"
#include "buffer/GGUniformBufferHandle.h"

#include "Math/GGMatrix4.h"

#if defined(__ANDROID__)
#include <android/asset_manager.h>
#endif
#include "GGRenderable.h"

namespace Gange {
class GGVulkanGlTFModel : public Renderable {
public:
    GGVulkanDevice *mVulkanDevice;
    VkQueue mCopyQueue;

    struct Node;

    struct Material {
        Vector4 baseColorFactor = Vector4(1.0f);
        uint32_t baseColorTextureIndex;
    };

    struct Texture {
        int32_t imageIndex;
    };

    struct Primitive {
        uint32_t firstIndex;
        uint32_t indexCount;
        int32_t materialIndex;
    };

    struct Mesh {
        std::vector<Primitive> primitives;
    };

    struct Node {
        Node *parent;
        std::vector<Node> children;
        Mesh mesh;
        Matrix4 matrix;
    };

    struct Vertex {
        Vector3 pos;
        Vector3 normal;
        Vector2 uv;
        Vector3 color;
    };

    struct AnimationChannel {
        std::string path;
        Node *node;
        uint32_t samplerIndex;
    };

    std::vector<GGTextureImage> images;
    std::vector<Texture> textures;
    std::vector<Material> materials;
    std::vector<Node> nodes;

    GGVulkanGlTFModel();

    ~GGVulkanGlTFModel();

    void initialize() override;

    void setupDescriptorSet() override;

    void loadglTFFile(std::string filename);
    void loadImages(tinygltf::Model &input);
    void loadTextures(tinygltf::Model &input);
    void loadMaterials(tinygltf::Model &input);
    void loadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, GGVulkanGlTFModel::Node *parent,
                  std::vector<uint32_t> &indexBuffer, std::vector<GGVulkanGlTFModel::Vertex> &vertexBuffer);
    void drawNode(VkCommandBuffer commandBuffer, GGVulkanGlTFModel::Node node);
    void draw(VkCommandBuffer commandBuffer);
};

}  // namespace Gange

#endif  //