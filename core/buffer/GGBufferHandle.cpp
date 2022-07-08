#include "GGBufferHandle.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {
GGBufferHandle::GGBufferHandle() {
    mBuffer = new GGVulkanBuffer();
    mVulkanDevice = VulkanSingleHandle::getVulkanDevicePtr();
}

GGBufferHandle::~GGBufferHandle() {
    if (mBuffer) {
        delete mBuffer;
        mBuffer = nullptr;
    }
}

GGVulkanBuffer *GGBufferHandle::getBuffer() const {
    return mBuffer;
}

void GGBufferHandle::generate() {}

}  // namespace Gange
