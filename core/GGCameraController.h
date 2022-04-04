#ifndef _GG_CAMERA_CONTROLLER_H_
#define _GG_CAMERA_CONTROLLER_H_
#include "GangeCamera.h"
#include "GGSingleton.h"
#include <Windows.h>

namespace Gange {

class GGCameraController {
public:
    bool mViewUpdated = false;

    GGCameraController();
    ~GGCameraController();

    void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    Camera *getCamera();

    Camera *getCameraSkybox();

    void handleMouseMove(Real x, Real y);

    Vector3 calcIntersectPoint(Ray &ray);

private:
    Vector2 mousePos;

    bool is3rd = false;

    struct {
        bool left = false;
        bool right = false;
        bool middle = false;
    } mouseButtons;

    Vector3 mButtonRot;

    Camera mCamera;
    Camera mSkyBoxCamera;
};
}  // namespace Gange

#endif