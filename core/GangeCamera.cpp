#include "GangeCamera.h"
#include "GGVulkanTools.h"

namespace Gange {
Camera::Camera() {
    mFOVy = Radian(Degree(45.0f));
    mNearDist = 1.0f;
    mFarDist = 256.0f;
    mAspect = 1.77778f;
    mViewportWidth = 1280.f;
    mViewportHeight = 720.f;
}

Camera::~Camera() {}

void Camera::setAspectRatio(Real x) {
    mAspect = x;
}

void Camera::setEye(Real x, Real y, Real z) {
    mEye.x = x;
    mEye.y = y;
    mEye.z = z;
}

void Camera::setWindowSize(Real x, Real y) {
    mViewportWidth = x;
    mViewportHeight = y;
    Real aspect = x / y;
    if (aspect != mAspect) {
        mAspect = aspect;
        updateProjMat();
    }
}
//-----------------------------------------------------------------------
void Camera::setEye(const Vector3 &vec) {
    mEye = vec;
}

void Camera::setTarget(Real x, Real y, Real z) {
    mTarget.x = x;
    mTarget.y = y;
    mTarget.z = z;
}

void Camera::setTarget(const Vector3 &vec) {
    mTarget = vec;
}

const Vector3 &Camera::getEye() {
    return mEye;
}

const Vector3 &Camera::getTarget() {
    return mTarget;
}
//-----------------------------------------------------------------------
void Camera::setFarClipDistance(Real farPlane) {
    mFarDist = farPlane;
}

//-----------------------------------------------------------------------
Real Camera::getFarClipDistance() const {
    return mFarDist;
}

//-----------------------------------------------------------------------
void Camera::setNearClipDistance(Real nearPlane) {
    assert(nearPlane >= 0);
    mNearDist = nearPlane;
}

//-----------------------------------------------------------------------
Real Camera::getNearClipDistance() const {
    return mNearDist;
}

void Camera::updateViewMat() {
    Vector3 f = mTarget - mEye;
    f.normalise();

    Vector3 u = mUp;
    u.normalise();

    Vector3 s = f.crossProduct(u);
    s.normalise();
    u = s.crossProduct(f);

    mViewMatrix[0][0] = s.x;
    mViewMatrix[1][0] = s.y;
    mViewMatrix[2][0] = s.z;
    mViewMatrix[0][1] = u.x;
    mViewMatrix[1][1] = u.y;
    mViewMatrix[2][1] = u.z;
    mViewMatrix[0][2] = -f.x;
    mViewMatrix[1][2] = -f.y;
    mViewMatrix[2][2] = -f.z;
    mViewMatrix[3][0] = -s.dotProduct(mEye);
    mViewMatrix[3][1] = -u.dotProduct(mEye);
    mViewMatrix[3][2] = f.dotProduct(mEye);
}

void Camera::updateProjMat() {

    Radian thetaY(mFOVy * 0.5f);
    Real tanThetaY = Math::Tan(thetaY);

    mProjMatrix[0][0] = 1.0f / (mAspect * tanThetaY);
    mProjMatrix[1][1] = 1.0f / (tanThetaY);
    mProjMatrix[2][3] = -1.0f;

#if 1
    mProjMatrix[2][2] = mFarDist / (mNearDist - mFarDist);
    mProjMatrix[3][2] = -(mFarDist * mNearDist) / (mFarDist - mNearDist);
#else
    mProjMatrix[2][2] = (mFarDist + mNearDist) / (mFarDist - mNearDist);
    mProjMatrix[3][2] = -(2.0f * mFarDist * mNearDist) / (mFarDist - mNearDist);
#endif
}

Ray Camera::createRayFromScreen(const Vector2 &pot) {
    Vector4 minWorld;
    Vector4 maxWorld;

    Vector4 screen(Real(pot.x), Real(pot.y), 0, 1);
    Vector4 screen1(Real(pot.x), Real(pot.y), 1, 1);

    unProject(screen, minWorld);
    unProject(screen1, maxWorld);

    Ray ray;
    ray.setOrigin(Vector3(minWorld.x, minWorld.y, minWorld.z));

    Vector3 dir(maxWorld.x - minWorld.x, maxWorld.y - minWorld.y, maxWorld.z - minWorld.z);
    dir.normalise();
    ray.setDirection(dir);
    return ray;
}

bool Camera::unProject(const Vector4 &screen, Vector4 &world) {
    Vector4 v;
    v.x = screen.x;
    v.y = screen.y;
    v.z = screen.z;
    v.w = 1.0f;

    // map from viewport to 0 - 1
    v.x = v.x / (Real) mViewportWidth;
    v.y = v.y / (Real) mViewportHeight;

    // map to range 0 to 1
    v.x = v.x * 2.0f - 1.0f;
    v.y = -(v.y * 2.0f - 1.0f);

    Matrix4 inverse = (mViewMatrix * mProjMatrix).inverse();

    v = v * inverse;
    if (v.w == 0.0f) {
        return false;
    }
    world = v / v.w;
    // world.y *= -1;
    return true;
}

void Camera::scaleCameraByPos(const Vector3 &pos, Real persent) {
    Vector3 dir = pos - mEye;
    Real dirLength = dir.length();
    dir.normalise();

    Real dis = dirLength * persent;

    Real disCam = (mTarget - mEye).length() * persent;

    Vector3 dir2 = mTarget - mEye;
    dir2.normalise();
    Vector3 dirCam = dir2;

    mEye = pos - dir * dis;
    mTarget = mEye + dirCam * disCam;

    updateViewMat();
}

const Matrix4 &Camera::getViewMat() {
    return mViewMatrix;
}

const Matrix4 &Camera::getProjMat() {
    return mProjMatrix;
}

void Camera::rotateView(const Vector3 &axis, Real angle) {
    Real len(0);
    Matrix4 mat = Matrix4::IDENTITY;
    if (axis.x != 0) {
        mat.rotateXYZ(angle, mRight);
    } else {
        mat.rotateXYZ(angle, axis);
    }
    Matrix4 mat2 = mat.transpose();

    mDirection = mat2 * mDirection;
    mDirection.normalise();

    mUp = mat2 * mUp;
    mUp.normalise();

    mRight = mDirection.crossProduct(mUp);
    mRight.normalise();

    len = (mEye - mTarget).length();

    mEye = mTarget - mDirection * len;
    updateViewMat();
}

}  // namespace Gange