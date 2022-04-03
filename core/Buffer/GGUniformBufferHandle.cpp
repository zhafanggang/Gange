#include "GGUniformBufferHandle.h"

namespace Gange {

GGUniformBufferHandle::GGUniformBufferHandle() {}

GGUniformBufferHandle::~GGUniformBufferHandle() {}

void GGUniformBufferHandle::setCameraController(GGCameraController *cameraController) {
    mCameraController = cameraController;
}

void GGUniformBufferHandle::generate() {
    VK_CHECK_RESULT(mVulkanDevice->createBuffer(
        sizeof(uboModel), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mBuffer, &uboModel));
    VK_CHECK_RESULT(mBuffer->map());
    updateUniformBuffers();
}

void GGUniformBufferHandle::updateUniformBuffers() {
    uboModel.projection = mCameraController->getCamera()->getProjMat();
    uboModel.modelView = mCameraController->getCamera()->getViewMat();
    memcpy(mBuffer->mapped, &uboModel, sizeof(uboModel));
}

}  // namespace Gange
