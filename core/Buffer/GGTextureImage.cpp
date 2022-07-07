#define STB_IMAGE_IMPLEMENTATION
#include "buffer/GGTextureImage.h"
#include "GGLogger.h"
#include "GGVulkanTools.h"
#include "GGVulkanInitializers.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {

GGTextureImage::GGTextureImage() {
    mVulkanDevice = VulkanSingleHandle::getVulkanDevicePtr();
    mQueue = VulkanSingleHandle::getVkQueue();
}

void GGTextureImage::setVulkanDeviceAndQueue(GGVulkanDevice *vulkan_device, VkQueue queue) {
    mVulkanDevice = vulkan_device;
    mQueue = queue;
}

GGTextureImage::~GGTextureImage() {
    mVulkanDevice = nullptr;
}

void GGTextureImage::loadCubeMap(const char *filePath) {}

void GGTextureImage::loadFromFile(const char *filePath, bool cubeFlag) {
unsigned char *pixels;
VkDeviceSize imageSize;
#if defined(__ANDROID__)
	AAsset* asset = AAssetManager_open(VulkanSingleHandle::getAssetManager(), filePath, AASSET_MODE_STREAMING);
	if (!asset) {
		GG_INFO("Could not load texture from");
	}
	imageSize =  AAsset_getLength(asset);
	pixels = new unsigned char[imageSize];
	AAsset_read(asset, pixels, imageSize);
	AAsset_close(asset);
#else
    int texWidth, texHeight, texChannels;
    unsigned char *pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    mWidth = static_cast<uint32_t>(texWidth);
    mHeight = static_cast<uint32_t>(texHeight);
    VkDeviceSize imageSize = mWidth * mHeight * 4;
#endif

    mMipLevels = 1;
    mFormat = VK_FORMAT_R8G8B8A8_UNORM;

    if (!pixels) {
        GG_INFO(filePath);
        throw std::runtime_error("failed to load texture image!");
    }
    if (cubeFlag) {
        // loadCubeMap();
    } else {
        loadFromPixels(pixels, imageSize);
    }
}

void GGTextureImage::create() {
    std::string file = getAssetPath() + "textures/lenna.jpg";
    loadFromFile(file.c_str());
}

void GGTextureImage::createImage(VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = mWidth;
    imageInfo.extent.height = mHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = mFormat;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(mVulkanDevice->logicalDevice, &imageInfo, nullptr, &mImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(mVulkanDevice->logicalDevice, mImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = mVulkanDevice->getMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(mVulkanDevice->logicalDevice, &allocInfo, nullptr, &mDeviceMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(mVulkanDevice->logicalDevice, mImage, mDeviceMemory, 0);
}

void GGTextureImage::copyBufferToImage(VkBuffer buffer, VkImage image) {
    VkCommandBuffer commandBuffer = mVulkanDevice->beginOnceCommand();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {mWidth, mHeight, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    mVulkanDevice->endOnceCommand(commandBuffer, mQueue);
}

void GGTextureImage::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = mVulkanDevice->beginOnceCommand();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = mImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    mVulkanDevice->endOnceCommand(commandBuffer, mQueue);
}

void GGTextureImage::createTextureImageView() {

    VkImageViewCreateInfo view = initializers::imageViewCreateInfo();
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = mFormat;
    view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    // The subresource range describes the set of mip levels (and array layers) that can be accessed through this image
    // view It's possible to create multiple image views for a single image referring to different (and/or overlapping)
    // ranges of the image
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.baseMipLevel = 0;
    view.subresourceRange.baseArrayLayer = 0;
    view.subresourceRange.layerCount = 1;
    // Linear tiling usually won't support mip maps
    // Only set mip map count if optimal tiling is used
    view.subresourceRange.levelCount = 1;
    // The view will be based on the texture's image
    view.image = mImage;
    VK_CHECK_RESULT(vkCreateImageView(mVulkanDevice->logicalDevice, &view, nullptr, &mImageView));
}

void GGTextureImage::createTextureSampler() {
    VkSamplerCreateInfo sampler = initializers::samplerCreateInfo();
    sampler.magFilter = VK_FILTER_LINEAR;
    sampler.minFilter = VK_FILTER_LINEAR;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.mipLodBias = 0.0f;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    // Set max level-of-detail to mip level count of the texture
    sampler.maxLod = 1.0f;
    // Enable anisotropic filtering
    // This feature is optional, so we must check if it's supported on the device
    if (mVulkanDevice->features.samplerAnisotropy) {
        // Use max. level of anisotropy for this example
        sampler.maxAnisotropy = mVulkanDevice->properties.limits.maxSamplerAnisotropy;
        sampler.anisotropyEnable = VK_TRUE;
    } else {
        // The device does not support anisotropic filtering
        sampler.maxAnisotropy = 1.0;
        sampler.anisotropyEnable = VK_FALSE;
    }
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    VK_CHECK_RESULT(vkCreateSampler(mVulkanDevice->logicalDevice, &sampler, nullptr, &mSampler));
}

void GGTextureImage::updateDescriptor() {
    mDescriptorImageInfo.sampler = mSampler;
    mDescriptorImageInfo.imageView = mImageView;
    mDescriptorImageInfo.imageLayout = mImageLayout;
}

void GGTextureImage::loadFromPixels(void *buffer, VkDeviceSize bufferSize, VkFilter filter,
                                    VkImageUsageFlags imageUsageFlags, VkImageLayout imageLayout) {
    GG_INFO("---DD1D---");
    assert(buffer);

    VkMemoryAllocateInfo memAllocInfo = initializers::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    // Use a separate command buffer for texture loading
    VkCommandBuffer copyCmd = mVulkanDevice->beginOnceCommand();

    // Create a host-visible staging buffer that contains the raw image data
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    VkBufferCreateInfo bufferCreateInfo = initializers::bufferCreateInfo();
    bufferCreateInfo.size = bufferSize;
    // This buffer is used as a transfer source for the buffer copy
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateBuffer(mVulkanDevice->logicalDevice, &bufferCreateInfo, nullptr, &stagingBuffer));
GG_INFO("---DD2D---");
    // Get memory requirements for the staging buffer (alignment, memory type bits)
    vkGetBufferMemoryRequirements(mVulkanDevice->logicalDevice, stagingBuffer, &memReqs);

    memAllocInfo.allocationSize = memReqs.size;
    // Get memory type index for a host visible buffer
    memAllocInfo.memoryTypeIndex = mVulkanDevice->getMemoryType(
        memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VK_CHECK_RESULT(vkAllocateMemory(mVulkanDevice->logicalDevice, &memAllocInfo, nullptr, &stagingMemory));
    VK_CHECK_RESULT(vkBindBufferMemory(mVulkanDevice->logicalDevice, stagingBuffer, stagingMemory, 0));

    // Copy texture data into staging buffer
    uint8_t *data;
    VK_CHECK_RESULT(vkMapMemory(mVulkanDevice->logicalDevice, stagingMemory, 0, memReqs.size, 0, (void **) &data));
    memcpy(data, buffer, bufferSize);
    vkUnmapMemory(mVulkanDevice->logicalDevice, stagingMemory);

    VkBufferImageCopy bufferCopyRegion = {};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = 0;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = mWidth;
    bufferCopyRegion.imageExtent.height = mHeight;
    bufferCopyRegion.imageExtent.depth = 1;
    bufferCopyRegion.bufferOffset = 0;

    // Create optimal tiled target image
    VkImageCreateInfo imageCreateInfo = initializers::imageCreateInfo();
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = mFormat;
    imageCreateInfo.mipLevels = mMipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent = {mWidth, mHeight, 1};
    imageCreateInfo.usage = imageUsageFlags;
    // Ensure that the TRANSFER_DST bit is set for staging
    if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
        imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    VK_CHECK_RESULT(vkCreateImage(mVulkanDevice->logicalDevice, &imageCreateInfo, nullptr, &mImage));

    vkGetImageMemoryRequirements(mVulkanDevice->logicalDevice, mImage, &memReqs);

    memAllocInfo.allocationSize = memReqs.size;

    memAllocInfo.memoryTypeIndex =
        mVulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK_RESULT(vkAllocateMemory(mVulkanDevice->logicalDevice, &memAllocInfo, nullptr, &mDeviceMemory));
    VK_CHECK_RESULT(vkBindImageMemory(mVulkanDevice->logicalDevice, mImage, mDeviceMemory, 0));

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = mMipLevels;
    subresourceRange.layerCount = 1;

    // Image barrier for optimal image (target)
    // Optimal image will be used as destination for the copy
    Vulkantools::setImageLayout(copyCmd, mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                subresourceRange);

    // Copy mip levels from staging buffer
    vkCmdCopyBufferToImage(copyCmd, stagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

    // Change texture image layout to shader read after all mip levels have been copied
    this->mImageLayout = imageLayout;
    Vulkantools::setImageLayout(copyCmd, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout, subresourceRange);

    mVulkanDevice->endOnceCommand(copyCmd, mQueue);

    // Clean up staging resources
    vkFreeMemory(mVulkanDevice->logicalDevice, stagingMemory, nullptr);
    vkDestroyBuffer(mVulkanDevice->logicalDevice, stagingBuffer, nullptr);

    // Create sampler
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = filter;
    samplerCreateInfo.minFilter = filter;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = 0.0f;
    samplerCreateInfo.maxAnisotropy = 1.0f;
    VK_CHECK_RESULT(vkCreateSampler(mVulkanDevice->logicalDevice, &samplerCreateInfo, nullptr, &mSampler));

    // Create image view
    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.pNext = NULL;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.format = mFormat;
    viewCreateInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                                 VK_COMPONENT_SWIZZLE_A};
    viewCreateInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.image = mImage;
    VK_CHECK_RESULT(vkCreateImageView(mVulkanDevice->logicalDevice, &viewCreateInfo, nullptr, &mImageView));

    // Update descriptor image info member that can be used for setting up descriptor sets
    updateDescriptor();
}

void GGTextureImage::loadFromPixels(void *pixels, VkDeviceSize bufferSize, VkFormat format, uint32_t texWidth,
                                    uint32_t texHeight, VkFilter filter, VkImageUsageFlags imageUsageFlags,
                                    VkImageLayout imageLayout) {
    mFormat = format;
    mWidth = texWidth;
    mHeight = texHeight;
    mMipLevels = 1;
    loadFromPixels(pixels, bufferSize, filter, imageUsageFlags, imageLayout);
}

}  // namespace Gange
