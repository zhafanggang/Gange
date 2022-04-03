#ifndef _GG_VULKAN_DEVICE_H_
#define _GG_VULKAN_DEVICE_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "buffer/GGVulkanBuffer.h"

namespace Gange {

struct GGVulkanDevice {
    explicit GGVulkanDevice(VkPhysicalDevice physicalDevice);

    ~GGVulkanDevice();

    /** @brief Physical device representation */
    VkPhysicalDevice physicalDevice;
    /** @brief Logical device representation (application's view of the device) */
    VkDevice logicalDevice;
    /** @brief Properties of the physical device including limits that the application can check against */
    VkPhysicalDeviceProperties properties;
    /** @brief Features of the physical device that an application can use to check if a feature is supported */
    VkPhysicalDeviceFeatures features;
    /** @brief Features that have been enabled for use on the physical device */
    VkPhysicalDeviceFeatures enabledFeatures;
    /** @brief Memory types and heaps of the physical device */
    VkPhysicalDeviceMemoryProperties memoryProperties;
    /** @brief Queue family properties of the physical device */
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    /** @brief List of extensions supported by the device */
    std::vector<std::string> supportedExtensions;
    /** @brief Default command pool for the graphics queue family index */
    VkCommandPool mCommandPool = VK_NULL_HANDLE;

    VkQueue mQueue;

    struct {
        uint32_t graphics;
        uint32_t compute;
        uint32_t transfer;
    } queueFamilyIndices;

    bool enableDebugMarkers = false;

    uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr) const;

    uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags) const;
    VkResult createLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures, std::vector<const char *> enabledExtensions,
                                 void *pNextChain, bool useSwapChain = true,
                                 VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
    VkResult createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                          VkBuffer *buffer, VkDeviceMemory *bufferMemory, void *data = nullptr);
    VkResult createBuffer(VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
                          GGVulkanBuffer *buffer, void *data = nullptr);

    VkCommandPool
        createCommandPool(uint32_t queueFamilyIndex,
                          VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VkCommandBuffer beginOnceCommand();

    void setCMD(VkCommandPool commandPool1) {
        mCommandPool = commandPool1;
    };

    void endOnceCommand(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool cmdPool, bool freeFlag = true);
    void endOnceCommand(VkCommandBuffer commandBuffer, VkQueue queue, bool freeFlag = true);
    void endOnceCommand(VkCommandBuffer commandBuffer, bool freeFlag = true);

    bool extensionSupported(std::string extension);
};

}  // namespace Gange

#endif