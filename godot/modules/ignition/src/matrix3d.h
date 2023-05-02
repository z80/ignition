
#ifndef __MATRIX3D_H_
#define __MATRIX3D_H_

#include "vector3d.h"
#include "core/string/ustring.h"

namespace Ign
{

/// 3x3 matrix for rotation and scaling.
class Matrix3d
{
public:
    /// Construct an identity matrix.
    Matrix3d() noexcept :
        m00_(1.0),
        m01_(0.0),
        m02_(0.0),
        m10_(0.0),
        m11_(1.0),
        m12_(0.0),
        m20_(0.0),
        m21_(0.0),
        m22_(1.0)
    {
    }

    /// Copy-construct from another matrix.
    Matrix3d(const Matrix3d& matrix) noexcept = default;

    /// Construct from values.
    Matrix3d(Float v00, Float v01, Float v02,
            Float v10, Float v11, Float v12,
            Float v20, Float v21, Float v22) noexcept :
        m00_(v00),
        m01_(v01),
        m02_(v02),
        m10_(v10),
        m11_(v11),
        m12_(v12),
        m20_(v20),
        m21_(v21),
        m22_(v22)
    {
    }

    /// Construct from a Float array.
    explicit Matrix3d(const Float* data) noexcept :
        m00_(data[0]),
        m01_(data[1]),
        m02_(data[2]),
        m10_(data[3]),
        m11_(data[4]),
        m12_(data[5]),
        m20_(data[6]),
        m21_(data[7]),
        m22_(data[8])
    {
    }

    /// Assign from another matrix.
    Matrix3d& operator =(const Matrix3d& rhs) noexcept = default;

    /// Test for equality with another matrix without epsilon.
    bool operator ==(const Matrix3d& rhs) const
    {
        const Float* leftData = Data();
        const Float* rightData = rhs.Data();

        for (unsigned i = 0; i < 9; ++i)
        {
            if (leftData[i] != rightData[i])
                return false;
        }

        return true;
    }

    /// Test for inequality with another matrix without epsilon.
    bool operator !=(const Matrix3d& rhs) const { return !(*this == rhs); }

    /// Multiply a Vector3d.
    Vector3d operator *(const Vector3d& rhs) const
    {
        return Vector3d(
            m00_ * rhs.x_ + m01_ * rhs.y_ + m02_ * rhs.z_,
            m10_ * rhs.x_ + m11_ * rhs.y_ + m12_ * rhs.z_,
            m20_ * rhs.x_ + m21_ * rhs.y_ + m22_ * rhs.z_
        );
    }

    /// Add a matrix.
    Matrix3d operator +(const Matrix3d& rhs) const
    {
        return Matrix3d(
            m00_ + rhs.m00_,
            m01_ + rhs.m01_,
            m02_ + rhs.m02_,
            m10_ + rhs.m10_,
            m11_ + rhs.m11_,
            m12_ + rhs.m12_,
            m20_ + rhs.m20_,
            m21_ + rhs.m21_,
            m22_ + rhs.m22_
        );
    }

    /// Subtract a matrix.
    Matrix3d operator -(const Matrix3d& rhs) const
    {
        return Matrix3d(
            m00_ - rhs.m00_,
            m01_ - rhs.m01_,
            m02_ - rhs.m02_,
            m10_ - rhs.m10_,
            m11_ - rhs.m11_,
            m12_ - rhs.m12_,
            m20_ - rhs.m20_,
            m21_ - rhs.m21_,
            m22_ - rhs.m22_
        );
    }

    /// Multiply with a scalar.
    Matrix3d operator *(Float rhs) const
    {
        return Matrix3d(
            m00_ * rhs,
            m01_ * rhs,
            m02_ * rhs,
            m10_ * rhs,
            m11_ * rhs,
            m12_ * rhs,
            m20_ * rhs,
            m21_ * rhs,
            m22_ * rhs
        );
    }

    /// Multiply a matrix.
    Matrix3d operator *(const Matrix3d& rhs) const
    {
        return Matrix3d(
            m00_ * rhs.m00_ + m01_ * rhs.m10_ + m02_ * rhs.m20_,
            m00_ * rhs.m01_ + m01_ * rhs.m11_ + m02_ * rhs.m21_,
            m00_ * rhs.m02_ + m01_ * rhs.m12_ + m02_ * rhs.m22_,
            m10_ * rhs.m00_ + m11_ * rhs.m10_ + m12_ * rhs.m20_,
            m10_ * rhs.m01_ + m11_ * rhs.m11_ + m12_ * rhs.m21_,
            m10_ * rhs.m02_ + m11_ * rhs.m12_ + m12_ * rhs.m22_,
            m20_ * rhs.m00_ + m21_ * rhs.m10_ + m22_ * rhs.m20_,
            m20_ * rhs.m01_ + m21_ * rhs.m11_ + m22_ * rhs.m21_,
            m20_ * rhs.m02_ + m21_ * rhs.m12_ + m22_ * rhs.m22_
        );
    }

    /// Set scaling elements.
    void SetScale(const Vector3d& scale)
    {
        m00_ = scale.x_;
        m11_ = scale.y_;
        m22_ = scale.z_;
    }

    /// Set uniform scaling elements.
    void SetScale(Float scale)
    {
        m00_ = scale;
        m11_ = scale;
        m22_ = scale;
    }

    /// Return the scaling part.
    Vector3d Scale() const
    {
        return Vector3d(
            sqrtf(m00_ * m00_ + m10_ * m10_ + m20_ * m20_),
            sqrtf(m01_ * m01_ + m11_ * m11_ + m21_ * m21_),
            sqrtf(m02_ * m02_ + m12_ * m12_ + m22_ * m22_)
        );
    }

    /// Return the scaling part with the sign. Reference rotation matrix is required to avoid ambiguity.
    Vector3d SignedScale(const Matrix3d& rotation) const
    {
        return Vector3d(
            rotation.m00_ * m00_ + rotation.m10_ * m10_ + rotation.m20_ * m20_,
            rotation.m01_ * m01_ + rotation.m11_ * m11_ + rotation.m21_ * m21_,
            rotation.m02_ * m02_ + rotation.m12_ * m12_ + rotation.m22_ * m22_
        );
    }

    /// Return transposed.
    Matrix3d Transpose() const
    {
        return Matrix3d(
            m00_,
            m10_,
            m20_,
            m01_,
            m11_,
            m21_,
            m02_,
            m12_,
            m22_
        );
    }

    /// Return scaled by a vector.
    Matrix3d Scaled(const Vector3d& scale) const
    {
        return Matrix3d(
            m00_ * scale.x_,
            m01_ * scale.y_,
            m02_ * scale.z_,
            m10_ * scale.x_,
            m11_ * scale.y_,
            m12_ * scale.z_,
            m20_ * scale.x_,
            m21_ * scale.y_,
            m22_ * scale.z_
        );
    }

    /// Test for equality with another matrix with epsilon.
    bool Equals(const Matrix3d& rhs) const
    {
        const Float* leftData = Data();
        const Float* rightData = rhs.Data();

        for (unsigned i = 0; i < 9; ++i)
        {
            if (!Urho3D::Equals(leftData[i], rightData[i]))
                return false;
        }

        return true;
    }

    /// Return inverse.
    Matrix3d Inverse() const;

    /// Return Float data.
    const Float* Data() const { return &m00_; }

    /// Return matrix element.
    Float Element(unsigned i, unsigned j) const { return Data()[i * 3 + j]; }

    /// Return matrix row.
    Vector3d Row(unsigned i) const { return Vector3d(Element(i, 0), Element(i, 1), Element(i, 2)); }

    /// Return matrix column.
    Vector3d Column(unsigned j) const { return Vector3d(Element(0, j), Element(1, j), Element(2, j)); }

    /// Return as string.
    String ToString() const;

    Float m00_;
    Float m01_;
    Float m02_;
    Float m10_;
    Float m11_;
    Float m12_;
    Float m20_;
    Float m21_;
    Float m22_;

    /// Bulk transpose matrices.
    static void BulkTranspose(Float* dest, const Float* src, unsigned count)
    {
        for (unsigned i = 0; i < count; ++i)
        {
            dest[0] = src[0];
            dest[1] = src[3];
            dest[2] = src[6];
            dest[3] = src[1];
            dest[4] = src[4];
            dest[5] = src[7];
            dest[6] = src[2];
            dest[7] = src[5];
            dest[8] = src[8];

            dest += 9;
            src += 9;
        }
    }

    /// Zero matrix.
    static const Matrix3d ZERO;
    /// Identity matrix.
    static const Matrix3d IDENTITY;
};

/// Multiply a 3x3 matrix with a scalar.
inline Matrix3d operator *(Float lhs, const Matrix3d& rhs) { return rhs * lhs; }

}



#endif


