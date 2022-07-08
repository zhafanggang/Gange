#ifndef _GG_Math_H_
#define _GG_Math_H_

#include "GGPrerequisites.h"
#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <iosfwd>
#include <cmath>

namespace Gange {
class Radian {
    Real mRad;

public:
    explicit Radian(Real r = 0)
        : mRad(r) {}
    Radian(const Degree &d);
    Radian &operator=(const Real &f) {
        mRad = f;
        return *this;
    }
    Radian &operator=(const Degree &d);

    Real valueDegrees() const;
    Real valueRadians() const {
        return mRad;
    }
    Real valueAngleUnits() const;

    const Radian &operator+() const {
        return *this;
    }
    Radian operator+(const Radian &r) const {
        return Radian(mRad + r.mRad);
    }
    Radian operator+(const Degree &d) const;
    Radian &operator+=(const Radian &r) {
        mRad += r.mRad;
        return *this;
    }
    Radian &operator+=(const Degree &d);
    Radian operator-() const {
        return Radian(-mRad);
    }
    Radian operator-(const Radian &r) const {
        return Radian(mRad - r.mRad);
    }
    Radian operator-(const Degree &d) const;
    Radian &operator-=(const Radian &r) {
        mRad -= r.mRad;
        return *this;
    }
    Radian &operator-=(const Degree &d);
    Radian operator*(Real f) const {
        return Radian(mRad * f);
    }
    Radian operator*(const Radian &f) const {
        return Radian(mRad * f.mRad);
    }
    Radian &operator*=(Real f) {
        mRad *= f;
        return *this;
    }
    Radian operator/(Real f) const {
        return Radian(mRad / f);
    }
    Radian &operator/=(Real f) {
        mRad /= f;
        return *this;
    }

    bool operator<(const Radian &r) const {
        return mRad < r.mRad;
    }
    bool operator<=(const Radian &r) const {
        return mRad <= r.mRad;
    }
    bool operator==(const Radian &r) const {
        return mRad == r.mRad;
    }
    bool operator!=(const Radian &r) const {
        return mRad != r.mRad;
    }
    bool operator>=(const Radian &r) const {
        return mRad >= r.mRad;
    }
    bool operator>(const Radian &r) const {
        return mRad > r.mRad;
    }

    _GGDllexport friend std::ostream &operator<<(std::ostream &o, const Radian &v);
};

class Degree {
    Real mDeg;

public:
    explicit Degree(Real d = 0)
        : mDeg(d) {}
    Degree(const Radian &r)
        : mDeg(r.valueDegrees()) {}
    Degree &operator=(const Real &f) {
        mDeg = f;
        return *this;
    }
    Degree &operator=(const Radian &r) {
        mDeg = r.valueDegrees();
        return *this;
    }

    Real valueDegrees() const {
        return mDeg;
    }
    Real valueRadians() const;  // see bottom of this file
    Real valueAngleUnits() const;

    const Degree &operator+() const {
        return *this;
    }
    Degree operator+(const Degree &d) const {
        return Degree(mDeg + d.mDeg);
    }
    Degree operator+(const Radian &r) const {
        return Degree(mDeg + r.valueDegrees());
    }
    Degree &operator+=(const Degree &d) {
        mDeg += d.mDeg;
        return *this;
    }
    Degree &operator+=(const Radian &r) {
        mDeg += r.valueDegrees();
        return *this;
    }
    Degree operator-() const {
        return Degree(-mDeg);
    }
    Degree operator-(const Degree &d) const {
        return Degree(mDeg - d.mDeg);
    }
    Degree operator-(const Radian &r) const {
        return Degree(mDeg - r.valueDegrees());
    }
    Degree &operator-=(const Degree &d) {
        mDeg -= d.mDeg;
        return *this;
    }
    Degree &operator-=(const Radian &r) {
        mDeg -= r.valueDegrees();
        return *this;
    }
    Degree operator*(Real f) const {
        return Degree(mDeg * f);
    }
    Degree operator*(const Degree &f) const {
        return Degree(mDeg * f.mDeg);
    }
    Degree &operator*=(Real f) {
        mDeg *= f;
        return *this;
    }
    Degree operator/(Real f) const {
        return Degree(mDeg / f);
    }
    Degree &operator/=(Real f) {
        mDeg /= f;
        return *this;
    }

    bool operator<(const Degree &d) const {
        return mDeg < d.mDeg;
    }
    bool operator<=(const Degree &d) const {
        return mDeg <= d.mDeg;
    }
    bool operator==(const Degree &d) const {
        return mDeg == d.mDeg;
    }
    bool operator!=(const Degree &d) const {
        return mDeg != d.mDeg;
    }
    bool operator>=(const Degree &d) const {
        return mDeg >= d.mDeg;
    }
    bool operator>(const Degree &d) const {
        return mDeg > d.mDeg;
    }

    _GGDllexport friend std::ostream &operator<<(std::ostream &o, const Degree &v);
};

class Angle {
    Real mAngle;

public:
    explicit Angle(Real angle)
        : mAngle(angle) {}
    operator Radian() const;
    operator Degree() const;
};

inline Radian::Radian(const Degree &d)
    : mRad(d.valueRadians()) {}
inline Radian &Radian::operator=(const Degree &d) {
    mRad = d.valueRadians();
    return *this;
}
inline Radian Radian::operator+(const Degree &d) const {
    return Radian(mRad + d.valueRadians());
}
inline Radian &Radian::operator+=(const Degree &d) {
    mRad += d.valueRadians();
    return *this;
}
inline Radian Radian::operator-(const Degree &d) const {
    return Radian(mRad - d.valueRadians());
}
inline Radian &Radian::operator-=(const Degree &d) {
    mRad -= d.valueRadians();
    return *this;
}

class _GGDllexport Math {
public:
    enum AngleUnit { AU_DEGREE, AU_RADIAN };

    class RandomValueProvider {
    public:
        virtual ~RandomValueProvider();

        virtual Real getRandomUnit() = 0;
    };

protected:
    static AngleUnit msAngleUnit;

    static int mTrigTableSize;

    static Real mTrigTableFactor;
    static Real *mSinTable;
    static Real *mTanTable;

    static RandomValueProvider *mRandProvider;

    void buildTrigTables();

    static Real SinTable(Real fValue);
    static Real TanTable(Real fValue);

public:
    Math(unsigned int trigTableSize = 4096);

    ~Math();

    static inline int IAbs(int iValue) {
        return (iValue >= 0 ? iValue : -iValue);
    }
    static inline int ICeil(float fValue) {
        return int(std::ceil(fValue));
    }
    static inline int IFloor(float fValue) {
        return int(std::floor(fValue));
    }
    static int ISign(int iValue);

    static inline Real Abs(Real fValue) {
        return std::abs(fValue);
    }

    static inline Degree Abs(const Degree &dValue) {
        return Degree(std::abs(dValue.valueDegrees()));
    }

    static inline Radian Abs(const Radian &rValue) {
        return Radian(std::abs(rValue.valueRadians()));
    }

    static Radian ACos(Real fValue);

    static Radian ASin(Real fValue);

    static inline Radian ATan(Real fValue) {
        return Radian(std::atan(fValue));
    }

    static inline Radian ATan2(Real fY, Real fX) {
        return Radian(std::atan2(fY, fX));
    }

    static inline Real Ceil(Real fValue) {
        return std::ceil(fValue);
    }

    static inline bool isNaN(Real f) {
        return f != f;
    }

    static inline Real Cos(const Radian &fValue, bool useTables = false) {
        return (!useTables) ? std::cos(fValue.valueRadians()) : SinTable(fValue.valueRadians() + HALF_PI);
    }

    static inline Real Cos(Real fValue, bool useTables = false) {
        return (!useTables) ? std::cos(fValue) : SinTable(fValue + HALF_PI);
    }

    static inline Real Exp(Real fValue) {
        return std::exp(fValue);
    }

    static inline Real Floor(Real fValue) {
        return std::floor(fValue);
    }

    static inline Real Log(Real fValue) {
        return std::log(fValue);
    }

    static const Real LOG2;

    static inline Real Log2(Real fValue) {
        return std::log2(fValue);
    }

    static inline Real LogN(Real base, Real fValue) {
        return std::log(fValue) / std::log(base);
    }

    static inline Real Pow(Real fBase, Real fExponent) {
        return std::pow(fBase, fExponent);
    }

    static Real Sign(Real fValue);

    static inline Radian Sign(const Radian &rValue) {
        return Radian(Sign(rValue.valueRadians()));
    }
    static inline Degree Sign(const Degree &dValue) {
        return Degree(Sign(dValue.valueDegrees()));
    }

    static inline float saturate(float t) {
        float tmp = std::max(t, 0.0f);
        tmp = std::min(tmp, 1.0f);
        return tmp;
    }
    static inline double saturate(double t) {
        double tmp = std::max(t, 0.0);
        tmp = std::min(tmp, 1.0);
        return tmp;
    }

    template <typename T, typename S> static FORCEINLINE T lerp(const T &a, const T &b, const S &w) {
        return a + w * (b - a);
    }

    static inline Real Sin(const Radian &fValue, bool useTables = false) {
        return (!useTables) ? std::sin(fValue.valueRadians()) : SinTable(fValue.valueRadians());
    }

    static inline Real Sin(Real fValue, bool useTables = false) {
        return (!useTables) ? std::sin(fValue) : SinTable(fValue);
    }

    static inline Real Sqr(Real fValue) {
        return fValue * fValue;
    }

    /** Square root function.
        @param fValue
            The value whose square root will be calculated.
     */
    static inline Real Sqrt(Real fValue) {
        return std::sqrt(fValue);
    }

    /** Square root function.
        @param fValue
            The value, in radians, whose square root will be calculated.
        @return
            The square root of the angle in radians.
     */
    static inline Radian Sqrt(const Radian &fValue) {
        return Radian(std::sqrt(fValue.valueRadians()));
    }

    /** Square root function.
        @param fValue
            The value, in degrees, whose square root will be calculated.
        @return
            The square root of the angle in degrees.
     */
    static inline Degree Sqrt(const Degree &fValue) {
        return Degree(std::sqrt(fValue.valueDegrees()));
    }

    /** Inverse square root i.e. 1 / Sqrt(x), good for vector
        normalisation.
        @param fValue
            The value whose inverse square root will be calculated.
    */
    static Real InvSqrt(Real fValue) {
        return 1.0f / std::sqrt(fValue);
    }

    /** Generate a random number of unit length.
        @return
            A random number in the range from [0,1].
    */
    static Real UnitRandom();

    /** Generate a random number within the range provided.
        @param fLow
            The lower bound of the range.
        @param fHigh
            The upper bound of the range.
        @return
            A random number in the range from [fLow,fHigh].
     */
    static Real RangeRandom(Real fLow, Real fHigh);

    /** Generate a random number in the range [-1,1].
        @return
            A random number in the range from [-1,1].
     */
    static Real SymmetricRandom();

    static void SetRandomValueProvider(RandomValueProvider *provider);

    /** Tangent function.
        @param fValue
            Angle in radians
        @param useTables
            If true, uses lookup tables rather than
            calculation - faster but less accurate.
    */
    static inline Real Tan(const Radian &fValue, bool useTables = false) {
        return (!useTables) ? std::tan(fValue.valueRadians()) : TanTable(fValue.valueRadians());
    }
    /** Tangent function.
        @param fValue
            Angle in radians
        @param useTables
            If true, uses lookup tables rather than
            calculation - faster but less accurate.
    */
    static inline Real Tan(Real fValue, bool useTables = false) {
        return (!useTables) ? std::tan(fValue) : TanTable(fValue);
    }

    static inline Real DegreesToRadians(Real degrees) {
        return degrees * fDeg2Rad;
    }
    static inline Real RadiansToDegrees(Real radians) {
        return radians * fRad2Deg;
    }

    /** These functions used to set the assumed angle units (radians or degrees)
         expected when using the Angle type.
    @par
         You can set this directly after creating a new Root, and also before/after resource
    creation, depending on whether you want the change to affect resource files.
    */
    static void setAngleUnit(AngleUnit unit);
    /** Get the unit being used for angles. */
    static AngleUnit getAngleUnit();

    /** Convert from the current AngleUnit to radians. */
    static Real AngleUnitsToRadians(Real units);
    /** Convert from radians to the current AngleUnit . */
    static Real RadiansToAngleUnits(Real radians);
    /** Convert from the current AngleUnit to degrees. */
    static Real AngleUnitsToDegrees(Real units);
    /** Convert from degrees to the current AngleUnit. */
    static Real DegreesToAngleUnits(Real degrees);

protected:
    static Vector2 octahedronMappingWrap(Vector2 v);

public:
    /// Takes a directional 3D vector n and returns a 2D value in range [0; 1]
    /// Vector 'n' doesn't have to be normalized
    static Vector2 octahedronMappingEncode(Vector3 n);
    /// Takes a 2D value in range [0; 1] and returns a unit-length 3D vector
    static Vector3 octahedronMappingDecode(Vector2 f);

    /// Return closest power of two not smaller than given number
    static uint32 ClosestPow2(uint32 x) {
        if (!(x & (x - 1u)))
            return x;
        while (x & (x + 1u))
            x |= (x + 1u);
        return x + 1u;
    }

    /** Checks whether a given point is inside a triangle, in a
         2-dimensional (Cartesian) space.
         @remarks
             The vertices of the triangle must be given in either
             trigonometrical (anticlockwise) or inverse trigonometrical
             (clockwise) order.
         @param p
             The point.
         @param a
             The triangle's first vertex.
         @param b
             The triangle's second vertex.
         @param c
             The triangle's third vertex.
         @return
             If the point resides in the triangle, <b>true</b> is
             returned.
         @par
             If the point is outside the triangle, <b>false</b> is
             returned.
     */
    static bool pointInTri2D(const Vector2 &p, const Vector2 &a, const Vector2 &b, const Vector2 &c);

    /** Checks whether a given 3D point is inside a triangle.
    @remarks
         The vertices of the triangle must be given in either
         trigonometrical (anticlockwise) or inverse trigonometrical
         (clockwise) order, and the point must be guaranteed to be in the
         same plane as the triangle
     @param p
         p The point.
     @param a
         The triangle's first vertex.
     @param b
         The triangle's second vertex.
     @param c
         The triangle's third vertex.
     @param normal
         The triangle plane's normal (passed in rather than calculated
         on demand since the caller may already have it)
     @return
         If the point resides in the triangle, <b>true</b> is
         returned.
     @par
         If the point is outside the triangle, <b>false</b> is
         returned.
     */
    static bool pointInTri3D(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c,
                             const Vector3 &normal);

    /** Compare 2 reals, using tolerance for inaccuracies.
     */
    static bool RealEqual(Real a, Real b, Real tolerance = std::numeric_limits<Real>::epsilon()) {
        return std::abs(b - a) <= tolerance;
    }

    /// Wraps the value 'v' into the range [0; 1)
    static Real Wrap(Real v) {
        Real unusedInt;

        Real fractPart = std::abs(std::modf(v, &unusedInt));
        if (fractPart < 0.0)
            fractPart = Real(1.0) + fractPart;

        return fractPart;
    }

    /** Calculates the tangent space vector for a given set of positions / texture coords. */
    static Vector3 calculateTangentSpaceVector(const Vector3 &position1, const Vector3 &position2,
                                               const Vector3 &position3, Real u1, Real v1, Real u2, Real v2, Real u3,
                                               Real v3);

    /** Calculate a face normal, including the w component which is the offset from the origin. */
    static Vector4 calculateFaceNormal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
    /** Calculate a face normal, no w-information. */
    static Vector3 calculateBasicFaceNormal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
    /** Calculate a face normal without normalize, including the w component which is the offset from
     * the origin. */
    static Vector4 calculateFaceNormalWithoutNormalize(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
    /** Calculate a face normal without normalize, no w-information. */
    static Vector3 calculateBasicFaceNormalWithoutNormalize(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);

    /** Generates a value based on the Gaussian (normal) distribution function
        with the given offset and scale parameters.
    */
    static Real gaussianDistribution(Real x, Real offset = 0.0f, Real scale = 1.0f);

    /** Clamp a value within an inclusive range. */
    template <typename T> static T Clamp(T val, T minval, T maxval) {
        assert(minval <= maxval && "Invalid clamp range");
        return std::max(std::min(val, maxval), minval);
    }

    static Matrix4 makeViewMatrix(const Vector3 &position, const Quaternion &orientation,
                                  const Matrix4 *reflectMatrix = 0);

    /// Returns Greatest Common Denominator
    static size_t gcd(size_t a, size_t b) {
        return b == 0u ? a : gcd(b, a % b);
    }
    /// Returns Least Common Multiple
    static size_t lcm(size_t a, size_t b) {
        return (a / gcd(a, b)) * b;
    }

    static const Real POS_INFINITY;
    static const Real NEG_INFINITY;
    static const Real PI;
    static const Real TWO_PI;
    static const Real HALF_PI;
    static const Real fDeg2Rad;
    static const Real fRad2Deg;
};

// these functions must be defined down here, because they rely on the
// angle unit conversion functions in class Math:

inline Real Radian::valueDegrees() const {
    return Math::RadiansToDegrees(mRad);
}

inline Real Radian::valueAngleUnits() const {
    return Math::RadiansToAngleUnits(mRad);
}

inline Real Degree::valueRadians() const {
    return Math::DegreesToRadians(mDeg);
}

inline Real Degree::valueAngleUnits() const {
    return Math::DegreesToAngleUnits(mDeg);
}

inline Angle::operator Radian() const {
    return Radian(Math::AngleUnitsToRadians(mAngle));
}

inline Angle::operator Degree() const {
    return Degree(Math::AngleUnitsToDegrees(mAngle));
}

inline Radian operator*(Real a, const Radian &b) {
    return Radian(a * b.valueRadians());
}

inline Radian operator/(Real a, const Radian &b) {
    return Radian(a / b.valueRadians());
}

inline Degree operator*(Real a, const Degree &b) {
    return Degree(a * b.valueDegrees());
}

inline Degree operator/(Real a, const Degree &b) {
    return Degree(a / b.valueDegrees());
}
/** @} */
/** @} */
}  // namespace Gange

#endif