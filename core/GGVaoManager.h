#ifndef _GG_VAO_MANAGER_H_
#define _GG_VAO_MANAGER_H_

#include "GGRenderable.h"
#include "GGSingleton.h"
#include <vector>

namespace Gange {

class VaoManager {
public:
    VaoManager();
    ~VaoManager();

    const Renderable &createRanderable();

    void setUniform(std::vector<GGUniformBufferHandle *> mUniformBufferVec);

    void setDlout(VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout);

    VkPipelineVertexInputStateCreateInfo getVertexInputState();

    void buildCommandBuffers(VkCommandBuffer commandBuffer);

    std::vector<Renderable *> mRenderVec;

    std::vector<GGUniformBufferHandle *> mUniformBufferVec;

    bool falg = false;

private:
    struct {
        VkPipelineVertexInputStateCreateInfo inputState;
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    } mVertexInputState;

    void setupVertexInputState();

    VkDescriptorSet mDescriptorSet;
    VkPipelineLayout mPipelineLayout;
};

}  // namespace Gange

#endif