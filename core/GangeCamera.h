#ifndef _GG_CAMERA_H_
#define _GG_CAMERA_H_

#include <Math/GGMatrix4.h>
#include "GangeRay.h"
#include <iostream>

namespace Gange {
class Camera {
public:
    Camera();

    ~Camera();

    void setAspectRatio(Real x);

    void setEye(Real x, Real y, Real z);

    void setEye(const Vector3 &vec);

    void setTarget(Real x, Real y, Real z);

    void setTarget(const Vector3 &vec);

    const Vector3 &getEye();

    const Vector3 &getTarget();

    void setWindowSize(Real x, Real y);

    //-----------------------------------------------------------------------
    void Camera::setFarClipDistance(Real farPlane);

    //-----------------------------------------------------------------------
    Real Camera::getFarClipDistance() const;

    //-----------------------------------------------------------------------
    void Camera::setNearClipDistance(Real nearPlane);

    //-----------------------------------------------------------------------
    Real Camera::getNearClipDistance() const;

    void scaleCameraByPos(const Vector3 &pos, Real persent);

    virtual void rotateView(const Vector3 &pos, Real angle);

    const Matrix4 &getProjMat();

    const Matrix4 &getViewMat();

    void updateViewMat();

    void updateProjMat();

protected:
    /// Whether to yaw around a fixed axis.
    bool mYawFixed = true;
    /// Fixed axis to yaw around
    Vector3 mYawFixedAxis = Vector3::UNIT_Z;
    /// y-direction field-of-view (default 45)
    Radian mFOVy;
    /// Far clip distance - default 10000
    Real mFarDist;
    /// Near clip distance - default 100
    Real mNearDist;
    /// x/y viewport ratio - default 1.3333
    Real mAspect;

    Real mFocalLength;

    Vector2 mFrustumOffset = Vector2::ZERO;

    Real mViewportWidth, mViewportHeight;

private:
    Vector3 mEye;
    Vector3 mTarget;
    Vector3 mUp = Vector3(0.0f, 1.0f, 0.01f);
    Vector3 mRight = Vector3(1.0f, 0.0f, 0.0f);
    Vector3 mDirection;

    Quaternion mOrientation;

    /// Derived orientation/position of the camera, including reflection
    mutable Quaternion mDerivedOrientation;
    mutable Vector3 mDerivedPosition;

    /// Real world orientation/position of the camera
    mutable Quaternion mRealOrientation;
    mutable Vector3 mRealPosition;

    bool mReflect = false;

    mutable Matrix4 mReflectMatrix;
    mutable Matrix4 mViewMatrix = Matrix4::IDENTITY;

    mutable Matrix4 mProjMatrix = Matrix4::ZERO;

    bool mFilpY = true;

public:
    Ray createRayFromScreen(const Vector2 &pot);

    bool unProject(const Vector4 &screen, Vector4 &world);
};
}  // namespace Gange
#endif