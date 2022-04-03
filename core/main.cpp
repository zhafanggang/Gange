#include "DemoWindows.h"

#if 1
int main() {
    demovulkan::DBWindow *demoWindow = new demovulkan::DBWindow(1280, 720);
    delete demoWindow;
    return 0;
}
#else
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    demovulkan::DBWindow *demoWindow = new demovulkan::DBWindow(hInstance, 1280, 720);
    delete demoWindow;
    return 0;
}

#endif
