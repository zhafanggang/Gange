#ifndef _GG_RENDERABLE_H_
#define _GG_RENDERABLE_H_

#include <vector>
#include "buffer/GGIndexBufferHandle.h"
#include "buffer/GGVertexBufferHandle.h"
#include "buffer/GGTextureImage.h"
#include "math/GGMatrix4.h"
#include "buffer/GGUniformBufferHandle.h"
#include "AndroidTools.h"

namespace Gange {

enum DescriptorBindingFlags { ImageBaseColor = 0x00000001, ImageNormalMap = 0x00000002 };
struct Node;

struct Skin
{
	std::string            name;
	Node *                 skeletonRoot = nullptr;
	std::vector<Vector4> inverseBindMatrices;
	std::vector<Node *>    joints;
	GGVulkanBuffer            ssbo;
	VkDescriptorSet        descriptorSet;
};

struct Material {
    Vector4 baseColorFactor = Vector4(1.0f);
    uint32_t baseColorTextureIndex;
};

struct Primitive {
    uint32_t firstIndex;
    uint32_t indexCount;
	uint32_t vertexCount;
	uint32_t firstVertex;
    uint32_t materialIndex;
};

struct Mesh {
    std::vector<Primitive*> primitives;
};

struct Node {
    Node *parent;
	uint32_t index;
    std::vector<Node*> children;
    Mesh *mesh;
	Vector3 translation{};
	Vector3 scale{ 1.0f };
	Quaternion rotation{};
	int32_t skin = -1;
	Matrix4 matrix;
	Matrix4 getLocalMatrix();
};

struct AnimationChannel {
    std::string path;
    Node *node;
    uint32_t samplerIndex;
};

struct AnimationSampler
{
	std::string interpolation;
	std::vector<float>inputs;
	std::vector<Vector4>outputsVec4;
};

struct Animation
{
	std::string name;
	std::vector<AnimationSampler> samplers;
	std::vector<AnimationChannel> channels;
	float start = std::numeric_limits<float>::max();
	float end = std::numeric_limits<float>::min();
	float currentTime = 0.0f;
};


class Renderable {

public:
    Renderable();

    virtual ~Renderable();

    virtual void initialize(){};

    void loadVao(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

    void loadTexture(const std::vector<std::string> &fileName);

    void loadTexture(const char *fileName);

    void setuniformBuffers(std::vector<GGUniformBufferHandle *> mUniformbuffers);

    virtual void buildCommandBuffers(VkCommandBuffer commandBuffer);

    VkBuffer mVertexBuffer = VK_NULL_HANDLE;
    VkBuffer mIndexBuffer = VK_NULL_HANDLE;

    uint32_t mIndexCount = 0;
    uint32_t mVertexCount = 0;

    std::vector<GGTextureImage *> mImages;
    std::vector<GGUniformBufferHandle *> mUniformbuffers;
    VkPipelineLayout mPipelineLayout;

protected:
    Matrix4 mModelMat = Matrix4::IDENTITY;
    bool mProtagonist = false;
    bool flag = false;
    VkDescriptorSet mDescriptorSet;
    VkDescriptorSetLayout mDescriptorSetLayoutImage = VK_NULL_HANDLE;
    VkDescriptorSetLayout mDescriptorSetLayoutUbo = VK_NULL_HANDLE;
	VkDescriptorSetLayout mDescriptorSetLayoutFragUbo = VK_NULL_HANDLE;

    std::vector<Node *> mNodes;

    GGVulkanDevice *mVulkanDevice = nullptr;
    std::vector<Vertex> mVextices;
    std::vector<uint32_t> mIndices;

    VkDeviceMemory mVertexMemory = VK_NULL_HANDLE;
    VkDeviceMemory mIndexMemory = VK_NULL_HANDLE;

    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;

    void setupDescriptorSet();
};

}  // namespace Gange

#endif