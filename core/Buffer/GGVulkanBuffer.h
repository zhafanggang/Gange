#ifndef _GG_VULKAN_BUFFER_H_
#define _GG_VULKAN_BUFFER_H_

#include <vector>
#include "vulkan/vulkan.h"
#include <assert.h>

namespace Gange {

struct GGVulkanBuffer {
    VkDevice device;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDescriptorBufferInfo descriptor;
    VkDeviceSize size = 0;
    VkDeviceSize alignment = 0;
    void *mapped = nullptr;
    /** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
    VkBufferUsageFlags usageFlags;
    /** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point)
     */
    VkMemoryPropertyFlags memoryPropertyFlags;
    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();
    VkResult bind(VkDeviceSize offset = 0);
    void setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void copyTo(void *data, VkDeviceSize size);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void destroy();
};
}  // namespace Gange

#endif  // GG_VULKAN_BUFFER_H
