#include "GGVulkanSingleHandle.h"

namespace Gange {
GGVulkanDevice GGVulkanSingleHandle::getVulkanDevice() {
    return *mVulkanDevice;
};

GGVulkanDevice *GGVulkanSingleHandle::getVulkanDevicePtr() {
    return mVulkanDevice;
};

void GGVulkanSingleHandle::setVulkanDevice(GGVulkanDevice *vulkanDevice, VkQueue queue) {
    mVulkanDevice = vulkanDevice;
    mQueue = queue;
};

VkQueue GGVulkanSingleHandle::getVkQueue() {
    return mQueue;
}

VkQueue GGVulkanSingleHandle::mQueue = VK_NULL_HANDLE;

GGVulkanDevice *GGVulkanSingleHandle::mVulkanDevice = nullptr;

}  // namespace Gange
