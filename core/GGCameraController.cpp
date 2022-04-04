#include "GGCameraController.h"

namespace Gange {
GGCameraController::GGCameraController() {

    mCamera.setEye(Vector3(100.0f, 100.0f, 100.0f));
    mCamera.setTarget(Vector3(0.0f, 0.0f, 0.0f));
    mCamera.setNearClipDistance(1.0f);
    mCamera.setFarClipDistance(5000.f);
    mCamera.updateViewMat();
    mCamera.updateProjMat();
}

GGCameraController::~GGCameraController() {}

Camera *GGCameraController::getCamera() {
    return &mCamera;
}

Camera *GGCameraController::getCameraSkybox() {
    return &mSkyBoxCamera;
}

void GGCameraController::handleMouseMove(Real x, Real y) {
    if (mouseButtons.left) {
        if (mCamera.mCameraType == Camera::CameraType::firstperson) {
            Vector2 pos(x, y);
            Ray ray0 = mCamera.createRayFromScreen(pos);
            Ray ray1 = mCamera.createRayFromScreen(mousePos);

            Vector3 pos0 = calcIntersectPoint(ray0);
            Vector3 pos1 = calcIntersectPoint(ray1);

            Vector3 offset = pos1 - pos0;

            Vector3 newEye = mCamera.getEye() + offset;
            Vector3 newTgt = mCamera.getTarget() + offset;

            mCamera.setEye(newEye);
            mCamera.setTarget(newTgt);
            mCamera.updateViewMat();

            mViewUpdated = true;
        }
    }
    if (mouseButtons.right) {

        Vector2 curPos(x, y);
        Vector2 offset = curPos - mousePos;

        mCamera.rotateViewY(offset.x * 0.5f);
        mCamera.rotateViewX(offset.y * 0.5f);

        mCamera.updateViewMat();
        mViewUpdated = true;
    }
    if (mouseButtons.middle) {
        mViewUpdated = true;
    }
    mousePos = Vector2((Real) x, (Real) y);
}

void GGCameraController::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            mousePos = Vector2((Real) LOWORD(lParam), (Real) HIWORD(lParam));
            mouseButtons.left = true;
            if (mCamera.mCameraType == Camera::CameraType::thirdperson) {
                Ray ray = mCamera.createRayFromScreen(mousePos);
                Vector3 orth = ray.getOrigin();
                float tm = (orth.y + 1) / ray.getDirection().y;
                Vector3 c = ray.getOrigin() + abs(tm) * ray.getDirection();
                mCamera.setMoveTo(c);
                mViewUpdated = true;
            }
            break;
        }

        case WM_RBUTTONDOWN: {
            mouseButtons.right = true;

            mousePos = Vector2((Real) LOWORD(lParam), (Real) HIWORD(lParam));

            Ray ray = mCamera.createRayFromScreen(mousePos);

            Vector3 pos = ray.getOrigin();

            Real tm = abs((pos.y - 0) / ray.getDirection().y);
            mButtonRot = ray.getPoint(tm);
            mButtonRot.y = 0;
            break;
        }
        case WM_MBUTTONDOWN:
            mousePos = Vector2((Real) LOWORD(lParam), (Real) HIWORD(lParam));
            mouseButtons.middle = true;
            break;
        case WM_LBUTTONUP:
            mouseButtons.left = false;
            break;
        case WM_RBUTTONUP:
            mouseButtons.right = false;
            break;
        case WM_MBUTTONUP:
            mouseButtons.middle = false;
            break;
        case WM_MOUSEWHEEL: {
            short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            Real persent = wheelDelta > 0 ? 1.1f : 0.9f;
            switch (mCamera.mCameraType) {
                case Camera::CameraType::firstperson: {
                    Ray ray = mCamera.createRayFromScreen(mousePos);
                    Vector3 pos = ray.getOrigin();
                    float tm = abs((pos.y - 0) / ray.getDirection().y);
                    Vector3 center = ray.getPoint(tm);
                    center.z = 0;
                    mCamera.scaleCameraByPos(center, persent);
                    break;
                };
                case Camera::CameraType::thirdperson: {
                    mCamera.setRadius(persent * mCamera.getRadius());
                    break;
                };
            }
            mViewUpdated = true;
            break;
        }
        case WM_MOUSEMOVE: {
            handleMouseMove((Real) LOWORD(lParam), (Real) HIWORD(lParam));
            break;
        }
        case WM_GETMINMAXINFO: {
            LPMINMAXINFO minMaxInfo = (LPMINMAXINFO) lParam;
            minMaxInfo->ptMinTrackSize.x = 64;
            minMaxInfo->ptMinTrackSize.y = 64;
            break;
        }
    }
}

Vector3 GGCameraController::calcIntersectPoint(Ray &ray) {
    Vector3 pos = ray.getOrigin();
    float tm = abs((pos.y) / ray.getDirection().y);
    Vector3 target = ray.getPoint(tm);
    return Vector3(target.x, 0.0f, target.z);
}

}  // namespace Gange
