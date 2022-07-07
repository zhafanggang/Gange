#ifndef _GG_RENDER_SYSTEM_BASE_H_
#define _GG_RENDER_SYSTEM_BASE_H_

#include <vector>
#include "GGVulkanGlTFModel.h"
#include "GGCameraController.h"
#include "GGPrerequisites.h"
#include "AndroidTools.h"

namespace Gange {

class GGRenderSystemBase {
public:
    virtual void initialize();
    virtual void prepare();
    virtual void render(){};
    virtual void windowResize(){};

    GGRenderSystemBase();
    virtual ~GGRenderSystemBase();

#if defined(__ANDROID__)
    void setNativeWindow(ANativeWindow *nativeWindow);

protected:
    ANativeWindow *mNativeWindow;
    
#elif _WIN32
    void setTarget(HINSTANCE hInstance, HWND window);

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    HWND windowHandle;
    HINSTANCE windowInstance;
#endif
    
protected:
    uint32_t mWindowWidth = 1280;
    uint32_t mWindowHeight = 720;

    bool mPreparedFlag = false;
    GGCameraController *mCameraController = nullptr;
};

}  // namespace Gange

#endif