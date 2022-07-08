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

#if defined(VK_USE_PLATFORM_WIN32_KHR)

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

#elif defined(__ANDROID__)
void GGRenderSystemBase::setNativeWindow(ANativeWindow *nativeWindow, int width, int height)
{
    this->mNativeWindow = nativeWindow;
    this->mWindowWidth = width;
    this->mWindowHeight = height;
}

void GGRenderSystemBase::touchEvent(int msgID, float x, float y)
{
    if (this) {
        mCameraController->touchEvent(msgID, x, y);
    }
}

#endif

}  // namespace Gange
