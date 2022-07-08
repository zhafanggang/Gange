#include "GGMath.h"
#include "GGVector2.h"
#include "GGVector3.h"
#include "GGVector4.h"
#include "GGMatrix4.h"
#include <sstream>

namespace Gange {
const Real Math::POS_INFINITY = std::numeric_limits<Real>::infinity();
const Real Math::NEG_INFINITY = -std::numeric_limits<Real>::infinity();
const Real Math::PI = Real(4.0 * std::atan(1.0));
const Real Math::TWO_PI = Real(2.0 * PI);
const Real Math::HALF_PI = Real(0.5 * PI);
const Real Math::fDeg2Rad = PI / Real(180.0);
const Real Math::fRad2Deg = Real(180.0) / PI;
const Real Math::LOG2 = std::log(Real(2.0));

int Math::mTrigTableSize;
Math::AngleUnit Math::msAngleUnit;

Real Math::mTrigTableFactor;
Real *Math::mSinTable = NULL;
Real *Math::mTanTable = NULL;

Math::RandomValueProvider *Math::mRandProvider = NULL;

//-----------------------------------------------------------------------
Math::Math(unsigned int trigTableSize) {
    msAngleUnit = AU_DEGREE;
    mTrigTableSize = static_cast<int>(trigTableSize);
    mTrigTableFactor = mTrigTableSize / Math::TWO_PI;

    // mSinTable = OGRE_ALLOC_T( Real, trigTableSize, MEMCATEGORY_GENERAL );
    // mTanTable = OGRE_ALLOC_T( Real, trigTableSize, MEMCATEGORY_GENERAL );

    buildTrigTables();
}

//-----------------------------------------------------------------------
Math::~Math() {
    // OGRE_FREE( mSinTable, MEMCATEGORY_GENERAL );
    //  OGRE_FREE( mTanTable, MEMCATEGORY_GENERAL );
}

//-----------------------------------------------------------------------
void Math::buildTrigTables() {
    // Build trig lookup tables
    // Could get away with building only PI sized Sin table but simpler this
    // way. Who cares, it'll ony use an extra 8k of memory anyway and I like
    // simplicity.
    Real angle;
    for (int i = 0; i < mTrigTableSize; ++i) {
        angle = Math::TWO_PI * i / mTrigTableSize;
        mSinTable[i] = std::sin(angle);
        mTanTable[i] = std::tan(angle);
    }
}
//-----------------------------------------------------------------------
Real Math::SinTable(Real fValue) {
    // Convert range to index values, wrap if required
    int idx;
    if (fValue >= 0) {
        idx = int(fValue * mTrigTableFactor) % mTrigTableSize;
    } else {
        idx = mTrigTableSize - (int(-fValue * mTrigTableFactor) % mTrigTableSize) - 1;
    }

    return mSinTable[idx];
}
//-----------------------------------------------------------------------
Real Math::TanTable(Real fValue) {
    // Convert range to index values, wrap if required
    int idx = int(fValue *= mTrigTableFactor) % mTrigTableSize;
    return mTanTable[idx];
}
//-----------------------------------------------------------------------
int Math::ISign(int iValue) {
    return (iValue > 0 ? +1 : (iValue < 0 ? -1 : 0));
}
//-----------------------------------------------------------------------
Radian Math::ACos(Real fValue) {
    if (-1.0 < fValue) {
        if (fValue < 1.0)
            return Radian(std::acos(fValue));
        else
            return Radian(0.0);
    } else {
        return Radian(PI);
    }
}
//-----------------------------------------------------------------------
Radian Math::ASin(Real fValue) {
    if (-1.0 < fValue) {
        if (fValue < 1.0)
            return Radian(std::asin(fValue));
        else
            return Radian(HALF_PI);
    } else {
        return Radian(-HALF_PI);
    }
}
//-----------------------------------------------------------------------
Real Math::Sign(Real fValue) {
    if (fValue > 0.0)
        return 1.0;

    if (fValue < 0.0)
        return -1.0;

    return 0.0;
}
//-----------------------------------------------------------------------
Real Math::UnitRandom() {
    if (mRandProvider)
        return mRandProvider->getRandomUnit();
    else
        return Real(rand()) / Real(RAND_MAX);
}

//-----------------------------------------------------------------------
Real Math::RangeRandom(Real fLow, Real fHigh) {
    return (fHigh - fLow) * UnitRandom() + fLow;
}

//-----------------------------------------------------------------------
Real Math::SymmetricRandom() {
    return 2.0f * UnitRandom() - 1.0f;
}

//-----------------------------------------------------------------------
void Math::SetRandomValueProvider(RandomValueProvider *provider) {
    mRandProvider = provider;
}

//-----------------------------------------------------------------------
void Math::setAngleUnit(Math::AngleUnit unit) {
    msAngleUnit = unit;
}
//-----------------------------------------------------------------------
Math::AngleUnit Math::getAngleUnit() {
    return msAngleUnit;
}
//-----------------------------------------------------------------------
Real Math::AngleUnitsToRadians(Real angleunits) {
    if (msAngleUnit == AU_DEGREE)
        return angleunits * fDeg2Rad;
    else
        return angleunits;
}

//-----------------------------------------------------------------------
Real Math::RadiansToAngleUnits(Real radians) {
    if (msAngleUnit == AU_DEGREE)
        return radians * fRad2Deg;
    else
        return radians;
}

//-----------------------------------------------------------------------
Real Math::AngleUnitsToDegrees(Real angleunits) {
    if (msAngleUnit == AU_RADIAN)
        return angleunits * fRad2Deg;
    else
        return angleunits;
}

//-----------------------------------------------------------------------
Real Math::DegreesToAngleUnits(Real degrees) {
    if (msAngleUnit == AU_RADIAN)
        return degrees * fDeg2Rad;
    else
        return degrees;
}
//-----------------------------------------------------------------------
Vector2 Math::octahedronMappingWrap(Vector2 v) {
    Vector2 retVal;
    retVal.x = (Real(1.0f) - Abs(v.y)) * (v.x >= 0 ? Real(1.0f) : -Real(1.0f));
    retVal.y = (Real(1.0f) - Abs(v.x)) * (v.y >= 0 ? Real(1.0f) : -Real(1.0f));
    return retVal;
}
//-----------------------------------------------------------------------
Vector2 Math::octahedronMappingEncode(Vector3 n) {
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    Vector2 nxy = n.z >= 0 ? n.xy() : octahedronMappingWrap(n.xy());
    nxy = nxy * Real(0.5f) + Real(0.5f);
    return nxy;
}
//-----------------------------------------------------------------------
Vector3 Math::octahedronMappingDecode(Vector2 f) {
    f = f * Real(2.0f) - Real(1.0f);

    // https://twitter.com/Stubbesaurus/status/937994790553227264
    Vector3 n = Vector3(f.x, f.y, Real(1.0f) - Abs(f.x) - Abs(f.y));
    float t = saturate(-n.z);
    n.x += n.x >= 0 ? -t : t;
    n.y += n.y >= 0 ? -t : t;
    return n.normalisedCopy();
}
//-----------------------------------------------------------------------
bool Math::pointInTri2D(const Vector2 &p, const Vector2 &a, const Vector2 &b, const Vector2 &c) {
    // Winding must be consistent from all edges for point to be inside
    Vector2 v1, v2;
    Real dot[3];
    bool zeroDot[3];

    v1 = b - a;
    v2 = p - a;

    // Note we don't care about normalisation here since sign is all we need
    // It means we don't have to worry about magnitude of cross products either
    dot[0] = v1.crossProduct(v2);
    zeroDot[0] = Math::RealEqual(dot[0], 0.0f, Real(1e-3));

    v1 = c - b;
    v2 = p - b;

    dot[1] = v1.crossProduct(v2);
    zeroDot[1] = Math::RealEqual(dot[1], 0.0f, Real(1e-3));

    // Compare signs (ignore colinear / coincident points)
    if (!zeroDot[0] && !zeroDot[1] && Math::Sign(dot[0]) != Math::Sign(dot[1])) {
        return false;
    }

    v1 = a - c;
    v2 = p - c;

    dot[2] = v1.crossProduct(v2);
    zeroDot[2] = Math::RealEqual(dot[2], 0.0f, Real(1e-3));
    // Compare signs (ignore colinear / coincident points)
    if ((!zeroDot[0] && !zeroDot[2] && Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
        (!zeroDot[1] && !zeroDot[2] && Math::Sign(dot[1]) != Math::Sign(dot[2]))) {
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------
bool Math::pointInTri3D(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &normal) {
    // Winding must be consistent from all edges for point to be inside
    Vector3 v1, v2;
    Real dot[3];
    bool zeroDot[3];

    v1 = b - a;
    v2 = p - a;

    // Note we don't care about normalisation here since sign is all we need
    // It means we don't have to worry about magnitude of cross products either
    dot[0] = v1.crossProduct(v2).dotProduct(normal);
    zeroDot[0] = Math::RealEqual(dot[0], 0.0f, Real(1e-3));

    v1 = c - b;
    v2 = p - b;

    dot[1] = v1.crossProduct(v2).dotProduct(normal);
    zeroDot[1] = Math::RealEqual(dot[1], 0.0f, Real(1e-3));

    // Compare signs (ignore colinear / coincident points)
    if (!zeroDot[0] && !zeroDot[1] && Math::Sign(dot[0]) != Math::Sign(dot[1])) {
        return false;
    }

    v1 = a - c;
    v2 = p - c;

    dot[2] = v1.crossProduct(v2).dotProduct(normal);
    zeroDot[2] = Math::RealEqual(dot[2], 0.0f, Real(1e-3));
    // Compare signs (ignore colinear / coincident points)
    if ((!zeroDot[0] && !zeroDot[2] && Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
        (!zeroDot[1] && !zeroDot[2] && Math::Sign(dot[1]) != Math::Sign(dot[2]))) {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------
Vector3 Math::calculateTangentSpaceVector(const Vector3 &position1, const Vector3 &position2, const Vector3 &position3,
                                          Real u1, Real v1, Real u2, Real v2, Real u3, Real v3) {
    // side0 is the vector along one side of the triangle of vertices passed in,
    // and side1 is the vector along another side. Taking the cross product of these returns the
    // normal.
    Vector3 side0 = position1 - position2;
    Vector3 side1 = position3 - position1;
    // Calculate face normal
    Vector3 normal = side1.crossProduct(side0);
    normal.normalise();
    // Now we use a formula to calculate the tangent.
    Real deltaV0 = v1 - v2;
    Real deltaV1 = v3 - v1;
    Vector3 tangent = deltaV1 * side0 - deltaV0 * side1;
    tangent.normalise();
    // Calculate binormal
    Real deltaU0 = u1 - u2;
    Real deltaU1 = u3 - u1;
    Vector3 binormal = deltaU1 * side0 - deltaU0 * side1;
    binormal.normalise();
    // Now, we take the cross product of the tangents to get a vector which
    // should point in the same direction as our normal calculated above.
    // If it points in the opposite direction (the dot product between the normals is less than
    // zero), then we need to reverse the s and t tangents. This is because the triangle has been
    // mirrored when going from tangent space to object space. reverse tangents if necessary
    Vector3 tangentCross = tangent.crossProduct(binormal);
    if (tangentCross.dotProduct(normal) < 0.0f) {
        tangent = -tangent;
        binormal = -binormal;
    }

    return tangent;
}

//-----------------------------------------------------------------------
Vector4 Math::calculateFaceNormal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
    Vector3 normal = calculateBasicFaceNormal(v1, v2, v3);
    // Now set up the w (distance of tri from origin
    return Vector4(normal.x, normal.y, normal.z, -(normal.dotProduct(v1)));
}
//-----------------------------------------------------------------------
Vector3 Math::calculateBasicFaceNormal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
    Vector3 normal = (v2 - v1).crossProduct(v3 - v1);
    normal.normalise();
    return normal;
}
//-----------------------------------------------------------------------
Vector4 Math::calculateFaceNormalWithoutNormalize(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
    Vector3 normal = calculateBasicFaceNormalWithoutNormalize(v1, v2, v3);
    // Now set up the w (distance of tri from origin)
    return Vector4(normal.x, normal.y, normal.z, -(normal.dotProduct(v1)));
}
//-----------------------------------------------------------------------
Vector3 Math::calculateBasicFaceNormalWithoutNormalize(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
    Vector3 normal = (v2 - v1).crossProduct(v3 - v1);
    return normal;
}
//-----------------------------------------------------------------------
Real Math::gaussianDistribution(Real x, Real offset, Real scale) {
    Real nom = Math::Exp(-Math::Sqr(x - offset) / (2 * Math::Sqr(scale)));
    Real denom = scale * Math::Sqrt(2 * Math::PI);

    return nom / denom;
}
//---------------------------------------------------------------------
Matrix4 Math::makeViewMatrix(const Vector3 &position, const Quaternion &orientation, const Matrix4 *reflectMatrix) {
    Matrix4 viewMatrix;

    // View matrix is:
    //
    //  [ Lx  Uy  Dz  Tx  ]
    //  [ Lx  Uy  Dz  Ty  ]
    //  [ Lx  Uy  Dz  Tz  ]
    //  [ 0   0   0   1   ]
    //
    // Where T = -(Transposed(Rot) * Pos)

    // This is most efficiently done using 3x3 Matrices
    Matrix3 rot;
    orientation.ToRotationMatrix(rot);

    // Make the translation relative to new axes
    Matrix3 rotT = rot.Transpose();

    Vector3 translation = position;

    translation.y *= -1.0f;

    Vector3 trans = rotT * translation;

    // Make final matrix
    viewMatrix = Matrix4::IDENTITY;
    viewMatrix = rotT;  // fills upper 3x3
    viewMatrix[0][3] = trans.x;
    viewMatrix[1][3] = trans.y;
    viewMatrix[2][3] = trans.z;

    // Deal with reflections
    if (reflectMatrix) {
        viewMatrix = viewMatrix * (*reflectMatrix);
    }

    return viewMatrix.transpose();
}

//---------------------------------------------------------------------
Math::RandomValueProvider::~RandomValueProvider() {}
//---------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Radian &v) {
    o << "Radian(" << v.valueRadians() << ")";
    return o;
}
//---------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Degree &v) {
    o << "Degree(" << v.valueDegrees() << ")";
    return o;
}
//-----------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Vector2 &v) {
    o << "Vector2(" << v.x << ", " << v.y << ")";
    return o;
}
//-----------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Vector3 &v) {
    o << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
    return o;
}
//-----------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Vector4 &v) {
    o << "Vector4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return o;
}
//-----------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Quaternion &q) {
    o << "Quaternion(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    return o;
}

//-----------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Matrix3 &mat) {
    // clang-format off
        o << "Matrix3(" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << ", "
                        << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << ", "
                        << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ")";
    // clang-format on
    return o;
}
//-----------------------------------------------------------------------
std::ostream &operator<<(std::ostream &o, const Matrix4 &mat) {
    o << "Matrix4(";
    for (size_t i = 0; i < 4; ++i) {
        o << " row" << (unsigned) i << "{";
        for (size_t j = 0; j < 4; ++j) {
            o << mat[i][j] << " ";
        }
        o << "}";
    }
    o << ")";
    return o;
}

}  // namespace Gange
