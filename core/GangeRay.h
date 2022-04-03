#ifndef _GANGE_RAY_H_
#define _GANGE_RAY_H_

#include "GGPrerequisites.h"
#include "Math/GGVector3.h"

namespace Gange {
class _GGDllexport Ray {
protected:
    Vector3 mOrigin;
    Vector3 mDirection;

public:
    Ray()
        : mOrigin(Vector3::ZERO)
        , mDirection(Vector3::UNIT_Z) {}
    Ray(const Vector3 &origin, const Vector3 &direction)
        : mOrigin(origin)
        , mDirection(direction) {}

    /** Sets the origin of the ray. */
    void setOrigin(const Vector3 &origin) {
        mOrigin = origin;
    }
    /** Gets the origin of the ray. */
    const Vector3 &getOrigin() const {
        return mOrigin;
    }

    /** Sets the direction of the ray. */
    void setDirection(const Vector3 &dir) {
        mDirection = dir;
    }
    /** Gets the direction of the ray. */
    const Vector3 &getDirection() const {
        return mDirection;
    }

    /** Gets the position of a point t units along the ray. */
    Vector3 getPoint(Real t) const {
        return Vector3(mOrigin + (mDirection * t));
    }

    /** Gets the position of a point t units along the ray. */
    Vector3 operator*(Real t) const {
        return getPoint(t);
    }

    // std::pair<bool, Real> intersects( const Plane &p ) const { return Math::intersects( *this, p ); }

    /*std::pair<bool, Real> intersects( const PlaneBoundedVolume &p ) const
    {
        return Math::intersects( *this, p.planes, p.outside == Plane::POSITIVE_SIDE );
    }*/

    /*  std::pair<bool, Real> intersects( const Sphere &s ) const
      {
          return Math::intersects( *this, s );
      }*/

    /*  std::pair<bool, Real> intersects( const AxisAlignedBox &box ) const
      {
          return Math::intersects( *this, box );
      }*/
};
/** @} */
/** @} */

}  // namespace Gange
#endif
