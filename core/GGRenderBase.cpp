#include "GGRenderBase.h"
#include "GGRenderSystemBase.h"
#include "GGRenderSystemVulkan.h"
#include "GGSingleton.h"
#include <mutex>

namespace Gange {

void GGRenderBase::initialize() {
    Singleton<GGRenderSystemVulkan>::Get()->initialize();
}

void GGRenderBase::render() {
    Singleton<GGRenderSystemVulkan>::Get()->render();
}

void GGRenderBase::setTarget(HINSTANCE hInstance, HWND window) {
    Singleton<GGRenderSystemVulkan>::Get()->setTarget(hInstance, window);
}

void GGRenderBase::prepare() {
    Singleton<GGRenderSystemVulkan>::Get()->prepare();
}

void GGRenderBase::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Singleton<GGRenderSystemVulkan>::Get()->handleMessages(hWnd, uMsg, wParam, lParam);
}

GGRenderBase::GGRenderBase() {}

GGRenderBase::~GGRenderBase() {}

}  // namespace Gange
