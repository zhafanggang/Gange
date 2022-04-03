#include "DBRenderBase.h"
#include "DemoWindows.h"

Drakensberg::DBRenderBase *render_ = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (render_ != NULL) {
        // render_->handleMessages(hWnd, uMsg, wParam, lParam);
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    render_ = new Drakensberg::DBRenderBase();

    HWND window = demovulkan::setupWindow(hInstance, WndProc, 1280, 720);

    render_->initialize();
    render_->setTarget(hInstance, window);
    render_->prepare();

    while (true) {
        Sleep(1000);
        render_->render();
    }

    delete (render_);
    return 0;
}
