#ifndef _GG_RENDER_PIPLINE_H_
#define _GG_RENDER_PIPLINE_H_

#include <vulkan/vulkan.h>
#include "GGFastArray.h"
#include "GGRenderable.h"

namespace Gange {
class GGRenderPipline {
public:
    explicit GGRenderPipline(Renderable *renderable, VkRenderPass renderPass, VkSampleCountFlagBits sampleCount);
    ~GGRenderPipline();

    void initialize();

    GGFastArray<VkPipeline> mPiplines;

    VkRenderPass mRenderPass;

    Renderable *mRenderable;

    VkSampleCountFlagBits mSampleCount;

    VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
};

}  // namespace Gange

#endif
