#ifndef _GG_RENDERABLE_H_
#define _GG_RENDERABLE_H_

#include "buffer/GGTextureImage.h"
#include "GGFastArray.h"
#include "buffer/GGIndexBufferHandle.h"
#include "buffer/GGVertexBufferHandle.h"
#include "buffer/GGUniformBufferHandle.h"
#include "GGCameraController.h"

namespace Gange {

class Renderable {

public:
    Renderable();

    virtual ~Renderable();

    virtual void initialize();

    virtual void update();

    void setCameraController(GGCameraController *cameraController);

    virtual void setupDescriptorSetLayout();

    virtual void setupDescriptorSet();

    VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);

    VkPipelineLayout mPipelineLayout;

    std::vector<VkShaderModule> mShaderModules;

    GGFastArray<VkPipelineShaderStageCreateInfo> mShaderStages;

    GGUniformBufferHandle *mUniformbuffer;
    GGIndexBufferHandle *mIndexBuffer;
    GGVertexBufferHandle *mVertexBuffer;

    GGFastArray<VkDescriptorSetLayout> mDescriptorSetLayouts;

    GGTextureImage *mTextureImage;

protected:
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    GGVulkanDevice *mVulkanDevice = nullptr;
    VkQueue mQueue;
};

}  // namespace Gange

#endif