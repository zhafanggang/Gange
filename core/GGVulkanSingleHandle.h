#ifndef _GG_VULKAN_SINGLE_HANDLE_H_
#define _GG_VULKAN_SINGLE_HANDLE_H_
#include "GGVulkanDevice.h"

namespace Gange {
class GGVulkanSingleHandle {
public:
    static GGVulkanDevice getVulkanDevice();

    static GGVulkanDevice *getVulkanDevicePtr();

    static void setVulkanDevice(GGVulkanDevice *vulkanDevice, VkQueue queue);

    static VkQueue getVkQueue();

private:
    static GGVulkanDevice *mVulkanDevice;

    static VkQueue mQueue;
};

}  // namespace Gange

#endif  // GG_VULKAN_SINGLE_HANDLE_H
