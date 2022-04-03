#include "GGRenderBase.h"
#include "GGRenderSystemVulkan.h"
#include <mutex>

namespace Gange {

void GGRenderBase::initialize() {
    if (nullptr == mRenderSystem) {
        GGRenderSystemVulkan *render_system_vulkan_ = new GGRenderSystemVulkan();
        mRenderSystem = (GGRenderSystemBase *) render_system_vulkan_;
    }
    mRenderSystem->initialize();
}

void GGRenderBase::render() {
    mRenderSystem->render();
}

void GGRenderBase::setTarget(HINSTANCE hInstance, HWND window) {
    mRenderSystem->setTarget(hInstance, window);
}

void GGRenderBase::prepare() {
    mRenderSystem->prepare();
}

void GGRenderBase::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    mRenderSystem->handleMessages(hWnd, uMsg, wParam, lParam);
}

GGRenderBase::GGRenderBase() {}

GGRenderBase::~GGRenderBase() {}

}  // namespace Gange
