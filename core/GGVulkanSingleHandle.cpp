#include "GGVulkanSingleHandle.h"

namespace Gange {

#if defined(__ANDROID__)

AAssetManager *VulkanSingleHandle::mAssetManager = nullptr;

void VulkanSingleHandle::setAssetManager(AAssetManager *assetManager)
{
    mAssetManager = assetManager;
}

AAssetManager *VulkanSingleHandle::getAssetManager()
{
    return mAssetManager;
}

#endif


GGVulkanDevice VulkanSingleHandle::getVulkanDevice() {
    return *mVulkanDevice;
};

GGVulkanDevice *VulkanSingleHandle::getVulkanDevicePtr() {
    return mVulkanDevice;
};

GGCameraController *VulkanSingleHandle::getCameraController()
{
	return mCameraController;
}

void VulkanSingleHandle::setCameraController(GGCameraController* cameraController)
{
	mCameraController = cameraController;
}

void VulkanSingleHandle::setVulkanDevice(GGVulkanDevice *vulkanDevice, VkQueue queue) {
    mVulkanDevice = vulkanDevice;
    mQueue = queue;
};

VkQueue VulkanSingleHandle::getVkQueue() {
    return mQueue;
}

VkQueue VulkanSingleHandle::mQueue = VK_NULL_HANDLE;

GGVulkanDevice *VulkanSingleHandle::mVulkanDevice = nullptr;

GGCameraController *VulkanSingleHandle::mCameraController = nullptr;

}  // namespace Gange
