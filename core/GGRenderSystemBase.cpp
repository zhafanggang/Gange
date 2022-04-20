#include "GGRenderSystemBase.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {

GGRenderSystemBase::GGRenderSystemBase() {
    mCameraController = new GGCameraController();
	VulkanSingleHandle::setCameraController(mCameraController);
}

GGRenderSystemBase::~GGRenderSystemBase() {
    delete mCameraController;
    mCameraController = nullptr;
}

void GGRenderSystemBase::initialize() {}

void GGRenderSystemBase::prepare() {}

void GGRenderSystemBase::setTarget(HINSTANCE hInstance, HWND window) {
    this->windowInstance = hInstance;
    this->windowHandle = window;
}

void GGRenderSystemBase::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (uMsg == WM_SIZE && (wParam != SIZE_MINIMIZED)) {
        if ((mPreparedFlag) && (wParam != SIZE_MINIMIZED)) {
            if (::IsWindow(hWnd)) {
                windowResize();
                mWindowWidth = static_cast<uint32_t> LOWORD(lParam);
                mWindowHeight = static_cast<uint32_t> HIWORD(lParam);
            }
        }
    }
    if (this) {
        mCameraController->handleMessages(hWnd, uMsg, wParam, lParam);
    }
}

}  // namespace Gange
