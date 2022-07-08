#include "GGRenderBase.h"
#include "GGRenderSystemVulkan.h"
#include "GGSingleton.h"
#include "GGVulkanSingleHandle.h"
#include <mutex>

namespace Gange {

void RenderBase::initialize() {
    Singleton<GGRenderSystemVulkan>::Get()->initialize();
}

void RenderBase::render() {
    Singleton<GGRenderSystemVulkan>::Get()->render();
}

void RenderBase::prepare() {
    Singleton<GGRenderSystemVulkan>::Get()->prepare();
}

#if defined(VK_USE_PLATFORM_WIN32_KHR)
void RenderBase::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Singleton<GGRenderSystemVulkan>::Get()->handleMessages(hWnd, uMsg, wParam, lParam);
}

void RenderBase::setTarget(HINSTANCE hInstance, HWND window) {
    Singleton<GGRenderSystemVulkan>::Get()->setTarget(hInstance, window);
}

#elif defined(__ANDROID__)
void RenderBase::setTarget(ANativeWindow *nativeWindow, AAssetManager *assetManager,int width, int height)
{
    VulkanSingleHandle::setAssetManager(assetManager);
    Singleton<GGRenderSystemVulkan>::Get()->setNativeWindow(nativeWindow,width,height);
}

void RenderBase::touchEvent(int msgID, float x, float y)
{
    Singleton<GGRenderSystemVulkan>::Get()->touchEvent(msgID, x, y);
}
#endif

RenderBase::RenderBase() {}

RenderBase::~RenderBase() {}

}  // namespace Gange
