#ifndef _GG_RENDER_BASE_H_
#define _GG_RENDER_BASE_H_

#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

namespace Gange {

class __declspec(dllexport) GGRenderBase {

public:
    void initialize();

#ifdef _WIN32
    void setTarget(HINSTANCE hInstance, HWND window);

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif // _WIN32
    void prepare();

    void render();

    GGRenderBase();

    ~GGRenderBase();
};

}  // namespace Gange

#endif