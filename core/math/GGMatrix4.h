#ifndef _GG_Matrix4_H_
#define _GG_Matrix4_H_

#include "GGPrerequisites.h"
#include "GGMatrix3.h"
#include "GGVector4.h"

namespace Gange {
class _GGDllexport Matrix4 {
protected:
    friend class ArrayMatrix4;
    friend class ArrayMatrixAf4x3;
    friend class SimpleMatrix4;
    friend class SimpleMatrixAf4x3;

    union {
        Real m[4][4];
        Real _m[16];
    };

public:
    inline Matrix4() {}

    inline Matrix4(Real m00, Real m01, Real m02, Real m03, Real m10, Real m11, Real m12, Real m13, Real m20, Real m21,
                   Real m22, Real m23, Real m30, Real m31, Real m32, Real m33) {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;
        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;
        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;
        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }

    inline Matrix4(const Real *arr) {
        memcpy(m, arr, 16 * sizeof(Real));
    }

    inline Matrix4(const Matrix3 &m3x3) {
        operator=(IDENTITY);
        operator=(m3x3);
    }

    inline Matrix4(const Quaternion &rot) {
        Matrix3 m3x3;
        rot.ToRotationMatrix(m3x3);
        operator=(IDENTITY);
        operator=(m3x3);
    }

    inline void swap(Matrix4 &other) {
        std::swap(m[0][0], other.m[0][0]);
        std::swap(m[0][1], other.m[0][1]);
        std::swap(m[0][2], other.m[0][2]);
        std::swap(m[0][3], other.m[0][3]);
        std::swap(m[1][0], other.m[1][0]);
        std::swap(m[1][1], other.m[1][1]);
        std::swap(m[1][2], other.m[1][2]);
        std::swap(m[1][3], other.m[1][3]);
        std::swap(m[2][0], other.m[2][0]);
        std::swap(m[2][1], other.m[2][1]);
        std::swap(m[2][2], other.m[2][2]);
        std::swap(m[2][3], other.m[2][3]);
        std::swap(m[3][0], other.m[3][0]);
        std::swap(m[3][1], other.m[3][1]);
        std::swap(m[3][2], other.m[3][2]);
        std::swap(m[3][3], other.m[3][3]);
    }

    inline Real *operator[](size_t iRow) {
        assert(iRow < 4);
        return m[iRow];
    }

    inline const Real *operator[](size_t iRow) const {
        assert(iRow < 4);
        return m[iRow];
    }

    inline void translate(Vector3 const &pos) {
        m[3][0] = pos.x;
        m[3][1] = pos.y;
        m[3][2] = pos.z;
    }

    inline void rotateXYZ(Real angle, Vector3 const &axis) {
        Radian a = Radian(Degree(angle));
        Real c = Math::Cos(a);
        Real s = Math::Sin(a);

        Vector3 temp = (Real(1) - c) * axis;

        m[0][0] = c + temp[0] * axis[0];
        m[1][0] = 0 + temp[0] * axis[1] + s * axis[2];
        m[2][0] = 0 + temp[0] * axis[2] - s * axis[1];
        m[3][0] = 0;

        m[0][1] = 0 + temp[1] * axis[0] - s * axis[2];
        m[1][1] = c + temp[1] * axis[1];
        m[2][1] = 0 + temp[1] * axis[2] + s * axis[0];
        m[3][1] = 0;

        m[0][2] = 0 + temp[2] * axis[0] + s * axis[1];
        m[1][2] = 0 + temp[2] * axis[1] - s * axis[0];
        m[2][2] = c + temp[2] * axis[2];
        m[3][2] = 0;

        m[0][3] = 0;
        m[1][3] = 0;
        m[2][3] = 0;
        m[3][3] = 1;
    }

    inline Matrix4 concatenate(const Matrix4 &m2) const {
        Matrix4 r;
        r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
        r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
        r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
        r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

        r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
        r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
        r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
        r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

        r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
        r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
        r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
        r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

        r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
        r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
        r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
        r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

        return r;
    }
    //#endif

    inline Matrix4 operator*(const Matrix4 &m2) const {
        return concatenate(m2);
    }

    inline Vector3 operator*(const Vector3 &v) const {
        return Vector3(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3],
                       m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
                       m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
    }

    inline Vector4 operator*(const Vector4 &v) const {
        return Vector4(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
                       m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                       m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                       m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
    }

    /** Matrix addition.
     */
    inline Matrix4 operator+(const Matrix4 &m2) const {
        Matrix4 r;

        r.m[0][0] = m[0][0] + m2.m[0][0];
        r.m[0][1] = m[0][1] + m2.m[0][1];
        r.m[0][2] = m[0][2] + m2.m[0][2];
        r.m[0][3] = m[0][3] + m2.m[0][3];

        r.m[1][0] = m[1][0] + m2.m[1][0];
        r.m[1][1] = m[1][1] + m2.m[1][1];
        r.m[1][2] = m[1][2] + m2.m[1][2];
        r.m[1][3] = m[1][3] + m2.m[1][3];

        r.m[2][0] = m[2][0] + m2.m[2][0];
        r.m[2][1] = m[2][1] + m2.m[2][1];
        r.m[2][2] = m[2][2] + m2.m[2][2];
        r.m[2][3] = m[2][3] + m2.m[2][3];

        r.m[3][0] = m[3][0] + m2.m[3][0];
        r.m[3][1] = m[3][1] + m2.m[3][1];
        r.m[3][2] = m[3][2] + m2.m[3][2];
        r.m[3][3] = m[3][3] + m2.m[3][3];

        return r;
    }

    /** Matrix subtraction.
     */
    inline Matrix4 operator-(const Matrix4 &m2) const {
        Matrix4 r;
        r.m[0][0] = m[0][0] - m2.m[0][0];
        r.m[0][1] = m[0][1] - m2.m[0][1];
        r.m[0][2] = m[0][2] - m2.m[0][2];
        r.m[0][3] = m[0][3] - m2.m[0][3];

        r.m[1][0] = m[1][0] - m2.m[1][0];
        r.m[1][1] = m[1][1] - m2.m[1][1];
        r.m[1][2] = m[1][2] - m2.m[1][2];
        r.m[1][3] = m[1][3] - m2.m[1][3];

        r.m[2][0] = m[2][0] - m2.m[2][0];
        r.m[2][1] = m[2][1] - m2.m[2][1];
        r.m[2][2] = m[2][2] - m2.m[2][2];
        r.m[2][3] = m[2][3] - m2.m[2][3];

        r.m[3][0] = m[3][0] - m2.m[3][0];
        r.m[3][1] = m[3][1] - m2.m[3][1];
        r.m[3][2] = m[3][2] - m2.m[3][2];
        r.m[3][3] = m[3][3] - m2.m[3][3];

        return r;
    }

    /** Tests 2 matrices for equality.
     */
    inline bool operator==(const Matrix4 &m2) const {
        if (m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
            m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
            m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
            m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
            return false;
        return true;
    }

    /** Tests 2 matrices for inequality.
     */
    inline bool operator!=(const Matrix4 &m2) const {
        if (m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
            m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
            m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
            m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
            return true;
        return false;
    }

    /** Assignment from 3x3 matrix.
     */
    inline void operator=(const Matrix3 &mat3) {
        m[0][0] = mat3.m[0][0];
        m[0][1] = mat3.m[0][1];
        m[0][2] = mat3.m[0][2];
        m[1][0] = mat3.m[1][0];
        m[1][1] = mat3.m[1][1];
        m[1][2] = mat3.m[1][2];
        m[2][0] = mat3.m[2][0];
        m[2][1] = mat3.m[2][1];
        m[2][2] = mat3.m[2][2];
    }

    inline Matrix4 transpose() const {
        return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1], m[3][1], m[0][2], m[1][2],
                       m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
    }

    /*
    -----------------------------------------------------------------------
    Translation Transformation
    -----------------------------------------------------------------------
    */
    /** Sets the translation transformation part of the matrix.
     */
    inline void setTrans(const Vector3 &v) {
        m[3][0] = v.x;
        m[3][1] = v.y;
        m[3][2] = v.z;
    }

    /** Extracts the translation transformation part of the matrix.
     */
    inline Vector3 getTrans() const {
        return Vector3(m[3][0], m[3][1], m[3][2]);
    }

    /** Builds a translation matrix
     */
    inline void makeTrans(const Vector3 &v) {
        m[0][0] = 1.0;
        m[0][1] = 0.0;
        m[0][2] = 0.0;
        m[0][3] = 0.0f;
        m[1][0] = 0.0;
        m[1][1] = 1.0;
        m[1][2] = 0.0;
        m[1][3] = 0.0;
        m[2][0] = 0.0;
        m[2][1] = 0.0;
        m[2][2] = 1.0;
        m[2][3] = 0.0;
        m[3][0] = v.x;
        m[3][1] = v.y;
        m[3][2] = v.z;
        m[3][3] = 1.0;
    }

    inline void makeTrans(Real tx, Real ty, Real tz) {
        m[0][0] = 1.0;
        m[0][1] = 0.0;
        m[0][2] = 0.0;
        m[0][3] = tx;
        m[1][0] = 0.0;
        m[1][1] = 1.0;
        m[1][2] = 0.0;
        m[1][3] = ty;
        m[2][0] = 0.0;
        m[2][1] = 0.0;
        m[2][2] = 1.0;
        m[2][3] = tz;
        m[3][0] = 0.0;
        m[3][1] = 0.0;
        m[3][2] = 0.0;
        m[3][3] = 1.0;
    }

    /** Gets a translation matrix.
     */
    inline static Matrix4 getTrans(const Vector3 &v) {
        Matrix4 r;

        r.m[0][0] = 1.0;
        r.m[0][1] = 0.0;
        r.m[0][2] = 0.0;
        r.m[0][3] = v.x;
        r.m[1][0] = 0.0;
        r.m[1][1] = 1.0;
        r.m[1][2] = 0.0;
        r.m[1][3] = v.y;
        r.m[2][0] = 0.0;
        r.m[2][1] = 0.0;
        r.m[2][2] = 1.0;
        r.m[2][3] = v.z;
        r.m[3][0] = 0.0;
        r.m[3][1] = 0.0;
        r.m[3][2] = 0.0;
        r.m[3][3] = 1.0;

        return r;
    }

    /** Gets a translation matrix - variation for not using a vector.
     */
    inline static Matrix4 getTrans(Real t_x, Real t_y, Real t_z) {
        Matrix4 r;

        r.m[0][0] = 1.0;
        r.m[0][1] = 0.0;
        r.m[0][2] = 0.0;
        r.m[0][3] = t_x;
        r.m[1][0] = 0.0;
        r.m[1][1] = 1.0;
        r.m[1][2] = 0.0;
        r.m[1][3] = t_y;
        r.m[2][0] = 0.0;
        r.m[2][1] = 0.0;
        r.m[2][2] = 1.0;
        r.m[2][3] = t_z;
        r.m[3][0] = 0.0;
        r.m[3][1] = 0.0;
        r.m[3][2] = 0.0;
        r.m[3][3] = 1.0;

        return r;
    }

    /*
    -----------------------------------------------------------------------
    Scale Transformation
    -----------------------------------------------------------------------
    */
    /** Sets the scale part of the matrix.
     */
    inline void setScale(const Vector3 &v) {
        m[0][0] = v.x;
        m[1][1] = v.y;
        m[2][2] = v.z;
    }

    /** Gets a scale matrix.
     */
    inline static Matrix4 getScale(const Vector3 &v) {
        Matrix4 r;
        r.m[0][0] = v.x;
        r.m[0][1] = 0.0;
        r.m[0][2] = 0.0;
        r.m[0][3] = 0.0;
        r.m[1][0] = 0.0;
        r.m[1][1] = v.y;
        r.m[1][2] = 0.0;
        r.m[1][3] = 0.0;
        r.m[2][0] = 0.0;
        r.m[2][1] = 0.0;
        r.m[2][2] = v.z;
        r.m[2][3] = 0.0;
        r.m[3][0] = 0.0;
        r.m[3][1] = 0.0;
        r.m[3][2] = 0.0;
        r.m[3][3] = 1.0;

        return r;
    }

    /** Gets a scale matrix - variation for not using a vector.
     */
    inline static Matrix4 getScale(Real s_x, Real s_y, Real s_z) {
        Matrix4 r;
        r.m[0][0] = s_x;
        r.m[0][1] = 0.0;
        r.m[0][2] = 0.0;
        r.m[0][3] = 0.0;
        r.m[1][0] = 0.0;
        r.m[1][1] = s_y;
        r.m[1][2] = 0.0;
        r.m[1][3] = 0.0;
        r.m[2][0] = 0.0;
        r.m[2][1] = 0.0;
        r.m[2][2] = s_z;
        r.m[2][3] = 0.0;
        r.m[3][0] = 0.0;
        r.m[3][1] = 0.0;
        r.m[3][2] = 0.0;
        r.m[3][3] = 1.0;

        return r;
    }

    /** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix.
    @param m3x3 Destination Matrix3
    */
    inline void extract3x3Matrix(Matrix3 &m3x3) const {
        m3x3.m[0][0] = m[0][0];
        m3x3.m[0][1] = m[0][1];
        m3x3.m[0][2] = m[0][2];
        m3x3.m[1][0] = m[1][0];
        m3x3.m[1][1] = m[1][1];
        m3x3.m[1][2] = m[1][2];
        m3x3.m[2][0] = m[2][0];
        m3x3.m[2][1] = m[2][1];
        m3x3.m[2][2] = m[2][2];
    }

    /** Determines if this matrix involves a scaling. */
    inline bool hasScale() const {
        // check magnitude of column vectors (==local axes)
        Real t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
        if (!Math::RealEqual(t, 1.0, (Real) 1e-04))
            return true;
        t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
        if (!Math::RealEqual(t, 1.0, (Real) 1e-04))
            return true;
        t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
        if (!Math::RealEqual(t, 1.0, (Real) 1e-04))
            return true;

        return false;
    }

    /** Determines if this matrix involves a negative scaling. */
    inline bool hasNegativeScale() const {
        return determinant() < 0;
    }

    /** Extracts the rotation / scaling part as a quaternion from the Matrix.
     */
    inline Quaternion extractQuaternion() const {
        Matrix3 m3x3;
        extract3x3Matrix(m3x3);
        return Quaternion(m3x3);
    }

    static const Matrix4 ZERO;
    static const Matrix4 ZEROAFFINE;
    static const Matrix4 IDENTITY;
    /** Useful little matrix which takes 2D clipspace {-1, 1} to {0,1}
        and inverts the Y. */
    static const Matrix4 CLIPSPACE2DTOIMAGESPACE;

    inline Matrix4 operator*(Real scalar) const {
        return Matrix4(scalar * m[0][0], scalar * m[0][1], scalar * m[0][2], scalar * m[0][3], scalar * m[1][0],
                       scalar * m[1][1], scalar * m[1][2], scalar * m[1][3], scalar * m[2][0], scalar * m[2][1],
                       scalar * m[2][2], scalar * m[2][3], scalar * m[3][0], scalar * m[3][1], scalar * m[3][2],
                       scalar * m[3][3]);
    }

    /** Function for writing to a stream.
     */
    _GGDllexport friend std::ostream &operator<<(std::ostream &o, const Matrix4 &mat);

    Matrix4 adjoint() const;
    Real determinant() const;
    Matrix4 inverse() const;

    /** Building a Matrix4 from orientation / scale / position.
    @remarks
        Transform is performed in the order scale, rotate, translation, i.e. translation is
    independent of orientation axes, scale does not affect size of translation, rotation and scaling
    are always centered on the origin.
    */
    void makeTransform(const Vector3 &position, const Vector3 &scale, const Quaternion &orientation);

    /** Building an inverse Matrix4 from orientation / scale / position.
    @remarks
        As makeTransform except it build the inverse given the same data as makeTransform, so
        performing -translation, -rotate, 1/scale in that order.
    */
    void makeInverseTransform(const Vector3 &position, const Vector3 &scale, const Quaternion &orientation);

    /** Decompose a Matrix4 to orientation / scale / position.
     */
    void decomposition(Vector3 &position, Vector3 &scale, Quaternion &orientation) const;

    /** Check whether or not the matrix is affine matrix.
        @remarks
            An affine matrix is a 4x4 matrix with row 3 equal to (0, 0, 0, 1),
            e.g. no projective coefficients.
    */
    inline bool isAffine() const {
        return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
    }

    /** Returns the inverse of the affine matrix.
        @note
            The matrix must be an affine matrix. @see Matrix4::isAffine.
    */
    Matrix4 inverseAffine() const;

    /** Concatenate two affine matrices.
        @note
            The matrices must be affine matrix. @see Matrix4::isAffine.
    */
    inline Matrix4 concatenateAffine(const Matrix4 &m2) const {
        assert(isAffine() && m2.isAffine());

        return Matrix4(m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0],
                       m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1],
                       m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2],
                       m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3],

                       m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0],
                       m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1],
                       m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2],
                       m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3],

                       m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0],
                       m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1],
                       m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2],
                       m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3],

                       0, 0, 0, 1);
    }

    /** 3-D Vector transformation specially for an affine matrix.
        @remarks
            Transforms the given 3-D vector by the 3x3 submatrix, without
            adding translation, as should be transformed directions and normals.
        @note
            The matrix must be an affine matrix. @see Matrix4::isAffine.
    */
    inline Vector3 transformDirectionAffine(const Vector3 &v) const {
        assert(isAffine());

        return Vector3(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z, m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                       m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
    }

    /** 3-D Vector transformation specially for an affine matrix.
        @remarks
            Transforms the given 3-D vector by the matrix, projecting the
            result back into <i>w</i> = 1.
        @note
            The matrix must be an affine matrix. @see Matrix4::isAffine.
    */
    inline Vector3 transformAffine(const Vector3 &v) const {
        assert(isAffine());

        return Vector3(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3],
                       m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
                       m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
    }

    /** 4-D Vector transformation specially for an affine matrix.
        @note
            The matrix must be an affine matrix. @see Matrix4::isAffine.
    */
    inline Vector4 transformAffine(const Vector4 &v) const {
        assert(isAffine());

        return Vector4(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
                       m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                       m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w, v.w);
    }
};

/* Removed from Vector4 and made a non-member here because otherwise
   OgreMatrix4.h and OgreVector4.h have to try to include and inline each
   other, which frankly doesn't work ;)
*/
inline Vector4 operator*(const Vector4 &v, const Matrix4 &mat) {
    return Vector4(v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0] + v.w * mat[3][0],
                   v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1] + v.w * mat[3][1],
                   v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2] + v.w * mat[3][2],
                   v.x * mat[0][3] + v.y * mat[1][3] + v.z * mat[2][3] + v.w * mat[3][3]);
}
/** @} */
/** @} */
}  // namespace Gange
#endif
