#ifndef _GG_VULKAN_TOOLS_H_
#define _GG_VULKAN_TOOLS_H_

#include <vector>
#include <fstream>
#include "math/GGMatrix4.h"
#include "AndroidTools.h"

const std::string getAssetPath();

namespace Gange {

namespace Vulkantools {

VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);

#if defined(__ANDROID__)
VkShaderModule loadShader(AAssetManager* assetManager, const char *fileName, VkDevice device);
#else
VkShaderModule loadShader(const char *fileName, VkDevice device);
#endif

void setImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout,
                    VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange,
                    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

void setImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask,
                    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

void convertProjectionMatrix(const Matrix4 &matrx, Matrix4 &dest);

}  // namespace Vulkantools
}  // namespace Gange

#endif