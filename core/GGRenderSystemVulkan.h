#ifndef _GG_RENDER_SYSTEM_VULKAN_H_
#define _GG_RENDER_SYSTEM_VULKAN_H_

#include <vector>
#include <string>
#include <optional>
#include "GGRenderSystemBase.h"
#include "GGVulkanDevice.h"
#include "GGVulkanSwapChain.h"
#include "GGRenderPipline.h"
#include "buffer/GGVulkanBuffer.h"

#include "GangePlane.h"
#define GG_ENABLE_SAMPLE_RATE_SHADING 1
#define GG_ENABLE_MULTI_SAMPLE 1

namespace Gange {

#define VERTEX_BUFFER_BIND_ID 0

class GGRenderSystemVulkan : public GGRenderSystemBase {
public:
    void initialize();

    void prepare();

    void render();

    void windowResize() override;

    GGRenderSystemVulkan();

    virtual ~GGRenderSystemVulkan();

private:
    bool enableValidationLayers = true;
    std::string title = "Vulkan Example";
    std::string name = "vulkanExample";
    uint32_t apiVersion = VK_API_VERSION_1_0;

    std::vector<std::string> supportedExtensions;
    std::vector<const char *> enabledExtensions;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    VkInstance instance;

    VkPhysicalDevice physicalDevice;

    VkDevice device;

    VkQueue queue;

    VkClearColorValue defaultClearColor = {{0.025f, 0.025f, 0.025f, 1.0f}};

    // Depth buffer format (selected during Vulkan initialization)
    VkFormat depthFormat;

    GGVulkanDevice *vulkanDevice = nullptr;

    GGVulkanSwapChain swapChain;

    Renderable *mRenderable = nullptr;

    GGVulkanGlTFModel *mGirlModel = nullptr;

    Plane *mPlane = nullptr;

    // List of available frame buffers (same as number of swap chain images)
    std::vector<VkFramebuffer> frameBuffers;

    struct {
        // Swap chain image presentation
        VkSemaphore presentComplete;
        // Command buffer submission and execution
        VkSemaphore renderComplete;
    } semaphores;

    struct {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depthStencil;

    // Contains command buffers and semaphores to be presented to the queue
    VkSubmitInfo submitInfo;

    void *deviceCreatepNextChain = nullptr;

    VkPhysicalDeviceFeatures enabledFeatures{};

    /** @brief Pipeline stages used to wait at for graphics queue submissions */
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    std::vector<const char *> enabledDeviceExtensions;

    // Stores physical device properties (for e.g. checking device limits)
    VkPhysicalDeviceProperties deviceProperties;
    // Stores the features available on the selected physical device (for e.g. checking if a feature is available)
    VkPhysicalDeviceFeatures deviceFeatures;
    // Stores all available memory (type) properties for the physical device
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

    // Command buffer pool
    VkCommandPool cmdPool;

    // Command buffers used for rendering
    std::vector<VkCommandBuffer> drawCmdBuffers;

    std::vector<VkFence> waitFences;

    // Global render pass for frame buffer writes
    VkRenderPass renderPass;

    // Active frame buffer index
    uint32_t currentBuffer = 0;

    void buildCommandBuffers();

    void destroyCommandBuffers();

    void createInstance();

    void pickPhysicalDevice();

    void initSwapchain();

    void createCommandPool();

    bool checkValidationLayerSupport();

    void setupSwapChain();

    void createSynchronizationPrimitives();

    void createCommandBuffers();

    void setupDepthStencil();

    void setupRenderPass();

    void setupFrameBuffer();

    void prepareFrame();

    void submitFrame();

    void draw();

    void getEnabledFeatures();

    void preparePipline();

    VkSampleCountFlagBits getMaxUsableSampleCount();

    struct {
        struct {
            VkImage image;
            VkImageView view;
            VkDeviceMemory memory;
        } color;
        struct {
            VkImage image;
            VkImageView view;
            VkDeviceMemory memory;
        } depth;
    } multisampleTarget;

    void setupMultisampleTarget();

    GGRenderPipline *mPipline = nullptr;

    VkSampleCountFlagBits mSampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);
};
}  // namespace Gange

#endif