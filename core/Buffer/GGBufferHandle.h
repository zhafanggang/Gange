#ifndef _GG_BUFFER_HANDLE_
#define _GG_BUFFER_HANDLE_
#include <stdint.h>
#include "GGVulkanBuffer.h"
#include "GGVulkanDevice.h"
#include "GGFastArray.h"
#include "GGLogger.h"
#include "GGVulkanInitializers.h"

namespace Gange {

enum GGBufferHandleType {
    GG_BUFFER_HANDLE_TYPE_ERROR = -1,
    GG_BUFFER_HANDLE_TYPE_VERTEX = 0,
    GG_BUFFER_HANDLE_TYPE_INDEX,
    GG_BUFFER_HANDLE_TYPE_UNIFORM,
    GG_BUFFER_HANDLE_TYPE_IMAGE,
};

class GGBufferHandle {

public:
    GGBufferHandle();

    virtual ~GGBufferHandle();

    GGVulkanBuffer *getBuffer() const;

    virtual void generate();

    VkDescriptorSet mDescriptorSet;

protected:
    GGVulkanDevice *mVulkanDevice = nullptr;
    GGVulkanBuffer *mBuffer = nullptr;
};

}  // namespace Gange

#endif