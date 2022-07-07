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
#include "buffer/GGUniformBufferHandle.h"
#include "math/GGMatrix4.h"
#include "GGRenderable.h"

namespace tinygltf {
class Model;
class Node;
};

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

    void initialize() override;

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