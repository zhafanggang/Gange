#ifndef _GG_RENDER_BASE_H_
#define _GG_RENDER_BASE_H_

#ifdef _WIN32
#include <windows.h>
#elif defined(__ANDROID__)
#include <android/native_window.h>
#include <android/asset_manager.h>
#endif

namespace Gange {

#ifdef _WIN32
class _GGDllexport RenderBase {
#else
class RenderBase {
#endif

public:
    void initialize();

#ifdef _WIN32
    void setTarget(HINSTANCE hInstance, HWND window);

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif defined(__ANDROID__)
    void setTarget(ANativeWindow *nativeWindow, AAssetManager *assetManager);
#endif // _WIN32

    void prepare();

    void render();

    RenderBase();

    ~RenderBase();
};

}  // namespace Gange

#endif