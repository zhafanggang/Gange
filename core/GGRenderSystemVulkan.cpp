#include "GGRenderSystemVulkan.h"
#include <iostream>
#include <assert.h>
#include <array>
#include "GGLogger.h"
#include "GGVulkanInitializers.h"
#include "GGVulkanTools.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {

GGRenderSystemVulkan::GGRenderSystemVulkan() {
    mRenderable = new Renderable();
}

void GGRenderSystemVulkan::destroyCommandBuffers() {
    vkFreeCommandBuffers(device, cmdPool, static_cast<uint32_t>(drawCmdBuffers.size()), drawCmdBuffers.data());
}

GGRenderSystemVulkan::~GGRenderSystemVulkan() {
    delete mRenderable;
    mRenderable = nullptr;

    delete mPlane;
    mPlane = nullptr;

    delete vulkanDevice;
}

void GGRenderSystemVulkan::buildCommandBuffers() {
    VkCommandBufferBeginInfo cmdBufInfo = initializers::commandBufferBeginInfo();

#if GG_ENABLE_MULTI_SAMPLE
    uint32_t clearValueCount = 3;
    VkClearValue clearValues[3];
    clearValues[0].color = {{.0f, .0f, .0f, 1.0f}};
    clearValues[1].color = {{.0f, .0f, .0f, 1.0f}};
    clearValues[2].depthStencil = {1.0f, 0};
#else
    uint32_t clearValueCount = 2;
    VkClearValue clearValues[2];
    clearValues[0].color = {{0.25f, 0.25f, 0.25f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
#endif

    VkRenderPassBeginInfo renderPassBeginInfo = initializers::renderPassBeginInfo();
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = mWindowWidth;
    renderPassBeginInfo.renderArea.extent.height = mWindowHeight;
    renderPassBeginInfo.clearValueCount = clearValueCount;
    renderPassBeginInfo.pClearValues = clearValues;

    const VkViewport viewport = initializers::viewport((float) mWindowWidth, (float) mWindowHeight, 0.0f, 1.0f);
    const VkRect2D scissor = initializers::rect2D(mWindowWidth, mWindowHeight, 0, 0);

    for (int32_t i = 0; i < drawCmdBuffers.size(); ++i) {
        renderPassBeginInfo.framebuffer = frameBuffers[i];
        VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));
        vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);
        vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);
        // Bind scene matrices descriptor to set 0
        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPlane->mPipelineLayout, 0, 1,
                                &mPlane->mUniformbuffer->mDescriptorSet, 0, nullptr);
        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipline->mPiplines[0]);
        mPlane->draw(drawCmdBuffers[i]);

        vkCmdEndRenderPass(drawCmdBuffers[i]);
        VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
    }
}

void GGRenderSystemVulkan::initialize() {
    GGRenderSystemBase::initialize();

    createInstance();
    pickPhysicalDevice();
    getEnabledFeatures();
    vulkanDevice = new GGVulkanDevice(physicalDevice);

    VkResult res = vulkanDevice->createLogicalDevice(enabledFeatures, enabledDeviceExtensions, deviceCreatepNextChain);
    if (res != VK_SUCCESS) {
        return;
    }
    device = vulkanDevice->logicalDevice;

    vkGetDeviceQueue(device, vulkanDevice->queueFamilyIndices.graphics, 0, &queue);

    GGVulkanSingleHandle::setVulkanDevice(vulkanDevice, queue);

    VkBool32 validDepthFormat = Vulkantools::getSupportedDepthFormat(physicalDevice, &depthFormat);
    assert(validDepthFormat);

    swapChain.connect(instance, physicalDevice, device);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.presentComplete));

    VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.renderComplete));

    submitInfo = initializers::submitInfo();
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = &submitPipelineStages;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &semaphores.presentComplete;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &semaphores.renderComplete;
}

void GGRenderSystemVulkan::prepare() {

#if GG_ENABLE_MULTI_SAMPLE
    mSampleCountFlagBits = getMaxUsableSampleCount();
#endif
    initSwapchain();
    createCommandPool();
    setupSwapChain();
    createCommandBuffers();
    createSynchronizationPrimitives();
    setupDepthStencil();
    setupRenderPass();
    setupFrameBuffer();

    mPlane = new Plane();
    mPlane->setCameraController(mCameraController);
    mPlane->initialize();

    preparePipline();

    buildCommandBuffers();

    mPreparedFlag = true;
}

void GGRenderSystemVulkan::setupFrameBuffer() {

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext = NULL;
    frameBufferCreateInfo.renderPass = renderPass;

    frameBufferCreateInfo.width = mWindowWidth;
    frameBufferCreateInfo.height = mWindowHeight;
    frameBufferCreateInfo.layers = 1;

#if GG_ENABLE_MULTI_SAMPLE
    VkImageView imageViews[3];
    setupMultisampleTarget();

    imageViews[0] = multisampleTarget.color.view;
    // attachment[1] = swapchain image
    imageViews[2] = multisampleTarget.depth.view;
    frameBufferCreateInfo.attachmentCount = 3;

#else
    VkImageView imageViews[2];
    // Depth/Stencil attachment is the same for all frame buffers
    imageViews[1] = depthStencil.view;
    frameBufferCreateInfo.attachmentCount = 2;
#endif
    frameBufferCreateInfo.pAttachments = imageViews;

    // Create frame buffers for every swap chain image
    frameBuffers.resize(swapChain.imageCount);
    for (uint32_t i = 0; i < frameBuffers.size(); i++) {
#if GG_ENABLE_MULTI_SAMPLE
        imageViews[1] = swapChain.buffers[i].view;
#else
        imageViews[0] = swapChain.buffers[i].view;
#endif
        vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &frameBuffers[i]);
    }
}

void GGRenderSystemVulkan::setupSwapChain() {
    swapChain.create(&mWindowWidth, &mWindowHeight);
}

void GGRenderSystemVulkan::createSynchronizationPrimitives() {
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    waitFences.resize(drawCmdBuffers.size());
    for (auto &fence : waitFences) {
        VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
    }
}

void GGRenderSystemVulkan::setupDepthStencil() {
    VkImageCreateInfo imageCI{};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.imageType = VK_IMAGE_TYPE_2D;
    imageCI.format = depthFormat;
    imageCI.extent = {mWindowWidth, mWindowHeight, 1};
    imageCI.mipLevels = 1;
    imageCI.arrayLayers = 1;
    imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    vkCreateImage(device, &imageCI, nullptr, &depthStencil.image);
    VkMemoryRequirements memReqs{};
    vkGetImageMemoryRequirements(device, depthStencil.image, &memReqs);

    VkMemoryAllocateInfo memAllloc{};
    memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllloc.allocationSize = memReqs.size;
    memAllloc.memoryTypeIndex =
        vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkAllocateMemory(device, &memAllloc, nullptr, &depthStencil.mem);
    vkBindImageMemory(device, depthStencil.image, depthStencil.mem, 0);

    VkImageViewCreateInfo imageViewCI{};
    imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCI.image = depthStencil.image;
    imageViewCI.format = depthFormat;
    imageViewCI.subresourceRange.baseMipLevel = 0;
    imageViewCI.subresourceRange.levelCount = 1;
    imageViewCI.subresourceRange.baseArrayLayer = 0;
    imageViewCI.subresourceRange.layerCount = 1;
    imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) {
        imageViewCI.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    vkCreateImageView(device, &imageViewCI, nullptr, &depthStencil.view);
}

void GGRenderSystemVulkan::setupRenderPass() {

    std::vector<VkAttachmentDescription> attachments;

    VkAttachmentDescription attachment;
    attachment.flags = 0;
    attachment.format = swapChain.colorFormat;
    attachment.samples = mSampleCountFlagBits;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

#if GG_ENABLE_MULTI_SAMPLE
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments.emplace_back(attachment);

    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
#endif
    // Color attachment
    attachments.emplace_back(attachment);
    // Depth attachment
    attachment.format = depthFormat;
    attachment.samples = mSampleCountFlagBits;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments.emplace_back(attachment);

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
#if GG_ENABLE_MULTI_SAMPLE
    depthReference.attachment = 2;
#else
    depthReference.attachment = 1;
#endif
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

#if GG_ENABLE_MULTI_SAMPLE
    VkAttachmentReference resolveReference = {};
    resolveReference.attachment = 1;
    resolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpassDescription.pResolveAttachments = &resolveReference;
#else
    subpassDescription.pResolveAttachments = nullptr;
#endif
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;

    VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
}

void GGRenderSystemVulkan::createCommandBuffers() {
    drawCmdBuffers.resize(swapChain.imageCount);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = cmdPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(drawCmdBuffers.size());
    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, drawCmdBuffers.data());
}

void GGRenderSystemVulkan::initSwapchain() {
#if defined(_WIN32)
    swapChain.initSurface(windowInstance, windowHandle);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
    swapChain.initSurface(androidApp->window);
#elif (defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
    swapChain.initSurface(view);
#endif
}

void GGRenderSystemVulkan::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char *> requiredExtensions = {VK_KHR_SURFACE_EXTENSION_NAME};

#if defined(_WIN32)
    requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
    requiredExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    requiredExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    requiredExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif

    if (enableValidationLayers) {
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VkResult po = vkCreateInstance(&createInfo, nullptr, &instance);
}

void GGRenderSystemVulkan::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    physicalDevice = devices[0];

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
}

void GGRenderSystemVulkan::getEnabledFeatures() {
    // Enable anisotropic filtering if supported
#if (GG_ENABLE_SAMPLE_RATE_SHADING)
    if (deviceFeatures.sampleRateShading) {
        enabledFeatures.sampleRateShading = VK_TRUE;
    }

#endif
    if (deviceFeatures.samplerAnisotropy) {
        enabledFeatures.samplerAnisotropy = VK_TRUE;
    };
}

bool GGRenderSystemVulkan::checkValidationLayerSupport() {
    std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void GGRenderSystemVulkan::createCommandPool() {
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = swapChain.queueNodeIndex;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK_RESULT(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &cmdPool));
}

void GGRenderSystemVulkan::preparePipline() {
    mPipline = new GGRenderPipline(mPlane, renderPass, mSampleCountFlagBits);
    mPipline->initialize();
}

VKAPI_ATTR VkBool32 VKAPI_CALL GGRenderSystemVulkan::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void GGRenderSystemVulkan::render() {

    if (mCameraController->mViewUpdated) {
        mCameraController->mViewUpdated = false;
        mPlane->update();
    }
    draw();
}

void GGRenderSystemVulkan::draw() {
    prepareFrame();

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];

    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    submitFrame();
}

void GGRenderSystemVulkan::prepareFrame() {

    VkResult result = swapChain.acquireNextImage(semaphores.presentComplete, &currentBuffer);
    if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {
    } else {
        VK_CHECK_RESULT(result);
    }
}

void GGRenderSystemVulkan::submitFrame() {
    VkResult result = swapChain.queuePresent(queue, currentBuffer, semaphores.renderComplete);
    if (!((result == VK_SUCCESS) || (result == VK_SUBOPTIMAL_KHR))) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            return;
        } else {
            VK_CHECK_RESULT(result);
        }
    }
    VK_CHECK_RESULT(vkQueueWaitIdle(queue));
}

void GGRenderSystemVulkan::windowResize() {
    if (!mPreparedFlag) {
        return;
    }
    mPreparedFlag = false;

    // Ensure all operations on the device have been finished before destroying resources
    vkDeviceWaitIdle(device);

    setupSwapChain();

    // Recreate the frame buffers
    vkDestroyImageView(device, depthStencil.view, nullptr);
    vkDestroyImage(device, depthStencil.image, nullptr);
    vkFreeMemory(device, depthStencil.mem, nullptr);
    setupDepthStencil();
    for (uint32_t i = 0; i < frameBuffers.size(); i++) {
        vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
    }
    setupFrameBuffer();

    // Command buffers need to be recreated as they may store
    // references to the recreated frame buffer
    destroyCommandBuffers();
    createCommandBuffers();
    buildCommandBuffers();

    vkDeviceWaitIdle(device);

    if ((mWindowWidth > 0.0f) && (mWindowWidth > 0.0f)) {
        mCameraController->getCamera()->setWindowSize((Real) mWindowWidth, (Real) mWindowHeight);
    }

    // mGirlModel->update();

    mPreparedFlag = true;
}

VkSampleCountFlagBits GGRenderSystemVulkan::getMaxUsableSampleCount() {
    VkSampleCountFlags counts = std::min(deviceProperties.limits.framebufferColorSampleCounts,
                                         deviceProperties.limits.framebufferDepthSampleCounts);
    if (counts & VK_SAMPLE_COUNT_64_BIT) {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT) {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT) {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT) {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT) {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT) {
        return VK_SAMPLE_COUNT_2_BIT;
    }
    return VK_SAMPLE_COUNT_1_BIT;
}

void GGRenderSystemVulkan::setupMultisampleTarget() {
    // Check if device supports requested sample count for color and depth frame buffer
    assert((deviceProperties.limits.framebufferColorSampleCounts >= (uint32_t) mSampleCountFlagBits) &&
           (deviceProperties.limits.framebufferDepthSampleCounts >= (uint32_t) mSampleCountFlagBits));

    // Color target
    VkImageCreateInfo info = initializers::imageCreateInfo();
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = swapChain.colorFormat;
    info.extent.width = mWindowWidth;
    info.extent.height = mWindowWidth;
    info.extent.depth = 1;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.samples = mSampleCountFlagBits;
    // Image will only be used as a transient target
    info.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK_RESULT(vkCreateImage(device, &info, nullptr, &multisampleTarget.color.image));

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device, multisampleTarget.color.image, &memReqs);
    VkMemoryAllocateInfo memAlloc = initializers::memoryAllocateInfo();
    memAlloc.allocationSize = memReqs.size;
    // We prefer a lazily allocated memory type
    // This means that the memory gets allocated when the implementation sees fit, e.g. when first using the images
    VkBool32 lazyMemTypePresent;
    memAlloc.memoryTypeIndex = vulkanDevice->getMemoryType(
        memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, &lazyMemTypePresent);
    if (!lazyMemTypePresent) {
        // If this is not available, fall back to device local memory
        memAlloc.memoryTypeIndex =
            vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &multisampleTarget.color.memory));
    vkBindImageMemory(device, multisampleTarget.color.image, multisampleTarget.color.memory, 0);

    // Create image view for the MSAA target
    VkImageViewCreateInfo viewInfo = initializers::imageViewCreateInfo();
    viewInfo.image = multisampleTarget.color.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapChain.colorFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &multisampleTarget.color.view));

    // Depth target
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = depthFormat;
    info.extent.width = mWindowWidth;
    info.extent.height = mWindowHeight;
    info.extent.depth = 1;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.samples = mSampleCountFlagBits;
    // Image will only be used as a transient target
    info.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK_RESULT(vkCreateImage(device, &info, nullptr, &multisampleTarget.depth.image));

    vkGetImageMemoryRequirements(device, multisampleTarget.depth.image, &memReqs);
    memAlloc = initializers::memoryAllocateInfo();
    memAlloc.allocationSize = memReqs.size;

    memAlloc.memoryTypeIndex = vulkanDevice->getMemoryType(
        memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, &lazyMemTypePresent);
    if (!lazyMemTypePresent) {
        memAlloc.memoryTypeIndex =
            vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    VK_CHECK_RESULT(vkAllocateMemory(device, &memAlloc, nullptr, &multisampleTarget.depth.memory));
    vkBindImageMemory(device, multisampleTarget.depth.image, multisampleTarget.depth.memory, 0);

    // Create image view for the MSAA target
    viewInfo.image = multisampleTarget.depth.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &multisampleTarget.depth.view));
}

}  // namespace Gange
