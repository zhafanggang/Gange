#ifndef _GG_RENDER_SYSTEM_BASE_H_
#define _GG_RENDER_SYSTEM_BASE_H_

#include "vulkan/vulkan.h"
#include <vector>
#include "GGVulkanGlTFModel.h"
#include "GGCameraController.h"
#include "GGPrerequisites.h"

namespace Gange {

class GGRenderSystemBase {
public:
    virtual void initialize();
    virtual void prepare();
    virtual void render(){};
    virtual void windowResize(){};

    GGRenderSystemBase();
    virtual ~GGRenderSystemBase();

    void setTarget(HINSTANCE hInstance, HWND window);

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    uint32_t mWindowWidth = 1280;
    uint32_t mWindowHeight = 720;

    bool mPreparedFlag = false;

    HINSTANCE windowInstance;
    HWND windowHandle;
    GGCameraController *mCameraController = nullptr;
};

}  // namespace Gange

#endif