#ifndef _GG_RENDER_BASE_H_
#define _GG_RENDER_BASE_H_

#include "GGRenderSystemBase.h"
#include "GGSingleton.h"

namespace Gange {

class GGRenderBase {

public:
    void initialize();

    void setTarget(HINSTANCE hInstance, HWND window);

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void prepare();

    void render();

    GGRenderBase();

    ~GGRenderBase();

private:
    GGRenderSystemBase *mRenderSystem = nullptr;
};

}  // namespace Gange

#endif