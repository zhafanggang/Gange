#ifndef _GG_TEXTURE_IMAGE_H_
#define _GG_TEXTURE_IMAGE_H_

#include <vulkan/vulkan.h>
#include "tinygltf/stb_image.h"
#include "GGVulkanDevice.h"

namespace Gange {
class GGTextureImage {
public:
    GGTextureImage();

    virtual ~GGTextureImage();

    virtual void create();

    void loadFromFile(const char *filePath, bool cubeFlag = false);

    void loadCubeMap(const char *filePath);

    std::pair<uint32_t, uint32_t> mSetLayoutBinding;

    VkDescriptorSet mDescriptorSet;
    void setVulkanDeviceAndQueue(GGVulkanDevice *vulkan_device, VkQueue queue);
    uint32_t mWidth, mHeight;
    uint32_t mMipLevels = 1;
    uint32_t mLayerCount;

    VkSampler mSampler;
    VkImage mImage;
    VkImageLayout mImageLayout;
    VkDeviceMemory mDeviceMemory;
    VkImageView mImageView;
    VkDescriptorImageInfo mDescriptorImageInfo;
    VkFormat mFormat;

    GGVulkanDevice *mVulkanDevice = nullptr;

    void loadFromPixels(void *buffer, VkDeviceSize bufferSize, VkFilter filter = VK_FILTER_LINEAR,
                        VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
                        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    void loadFromPixels(void *pixels, VkDeviceSize bufferSize, VkFormat format, uint32_t texWidth, uint32_t texHeight,
                        VkFilter filter = VK_FILTER_LINEAR,
                        VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
                        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

protected:
    VkQueue mQueue;

    void updateDescriptor();

private:
    void copyBufferToImage(VkBuffer buffer, VkImage image);

    void createImage(VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

    void createTextureImageView();

    void createTextureSampler();
};
}  // namespace Gange

#endif  // !GG_TEXTURE_H_
