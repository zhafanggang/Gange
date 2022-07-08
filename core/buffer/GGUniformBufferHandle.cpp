#include "GGUniformBufferHandle.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {

	float angle = 0.0f;

GGUniformBufferHandle::GGUniformBufferHandle(UniformType uniformType, bool protagonist)
    : mUniformType(uniformType) ,mProtagonist(protagonist){
    mCameraController = VulkanSingleHandle::getCameraController();
}

GGUniformBufferHandle::~GGUniformBufferHandle() {}

void GGUniformBufferHandle::generate() {
	if (mUniformType == UniformType::scene)
	{
		VK_CHECK_RESULT(mVulkanDevice->createBuffer(
			sizeof(mUboModel), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mBuffer, &mUboModel));
		VK_CHECK_RESULT(mBuffer->map());
	}
	else
	{
		VK_CHECK_RESULT(mVulkanDevice->createBuffer(
			sizeof(mUboLight), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mBuffer, &mUboLight));
		VK_CHECK_RESULT(mBuffer->map());
	}
	updateUniformBuffers();
}

bool GGUniformBufferHandle::getIsSkyBoxFlag() {
    return mProtagonist;
}

void GGUniformBufferHandle::updateUniformBuffers() {

	if (mUniformType == UniformType::scene)
	{
		mUboModel.projection = mCameraController->getCamera()->getProjMat();
		mUboModel.view = mCameraController->getCamera()->getViewMat();

		if (mProtagonist) {
			if (VulkanSingleHandle::getCameraController()->getCamera()->getMovedStatus()) {
				mUboModel.model.setTrans(VulkanSingleHandle::getCameraController()->getCamera()->getRolePos());
			}
		}
		memcpy(mBuffer->mapped, &mUboModel, sizeof(mUboModel));
	}
	else
	{
		angle += 0.001f;
		Real currentAngle = std::fmodf(angle, 360.f);
		mUboLight.viewPos = mCameraController->getCamera()->getEye();
		mUboLight.lightPos = Vector3(200.0f* Math::Cos(currentAngle), 100.0f, 200.0f* Math::Sin(currentAngle));
		memcpy(mBuffer->mapped, &mUboLight, sizeof(mUboLight));
	}
}


}  // namespace Gange
