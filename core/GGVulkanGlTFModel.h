#ifndef _GG_VULKAN_GLTF_MODEL_H_
#define _GG_VULKAN_GLTF_MODEL_H_

#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <limits>

#include "vulkan/vulkan.h"
#include "GGVulkanDevice.h"

#include "Buffer/GGTextureImage.h"
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
    std::vector<int32_t> mTextureIndices;
    std::vector<Material> mMaterials;
    std::vector<Node*> mNodes;
	std::vector<Node*> mLinearNodes;

	std::vector<Skin>      mSkins;
	std::vector<Animation> mAnimations;

    GGVulkanGlTFModel(bool protagonist = false);

    ~GGVulkanGlTFModel();

    void initialize();

    void loadglTFFile(std::string filename);
    void loadImages(tinygltf::Model &input);
    void loadTextures(tinygltf::Model &input);
    void loadMaterials(tinygltf::Model &input);

	void loadSkins(tinygltf::Model &input);
	void loadAnimations(tinygltf::Model &input);
	void updateJoints(Node *node);
	Node *findNode(Node *parent, uint32_t index);
	Node *nodeFromIndex(uint32_t index);
    void loadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, Node *parent,
                  std::vector<uint32_t> &indexBuffer, std::vector<Vertex> &vertexBuffer);
    void drawNode(VkCommandBuffer commandBuffer, Node *node);
	Matrix4 getNodeMatrix(Node *node);

    void buildCommandBuffers(VkCommandBuffer commandBuffer) override;
};

}  // namespace Gange

#endif  //