#ifndef _GG_VULKAN_SINGLE_HANDLE_H_
#define _GG_VULKAN_SINGLE_HANDLE_H_
#include "GGVulkanDevice.h"
#include "GGCameraController.h"
#include "AndroidTools.h"

namespace Gange {
class VulkanSingleHandle {
public:
    static GGVulkanDevice getVulkanDevice();

    static GGVulkanDevice *getVulkanDevicePtr();

	static GGCameraController* getCameraController();

	static void setCameraController(GGCameraController* cameraController);

    static void setVulkanDevice(GGVulkanDevice *vulkanDevice, VkQueue queue);

    static VkQueue getVkQueue();

#if defined(__ANDROID__)
    static void setAssetManager(AAssetManager *assetManager);

    static AAssetManager *getAssetManager();

private:

    static AAssetManager *mAssetManager;
#endif

private:

    static GGVulkanDevice *mVulkanDevice;

    static VkQueue mQueue;

	static GGCameraController* mCameraController;
};

}  // namespace Gange

#endif  // GG_VULKAN_SINGLE_HANDLE_H
