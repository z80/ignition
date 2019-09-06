
#ifndef __QUATERNION_D_H_
#define __QUATERNION_D_H_

#include "matrix3d.h"
#include "vector3d.h"
#include "data_types.h"
#include "Urho3D/Math/Quaternion.h"

using namespace Urho3D;

namespace Osp
{

/// Rotation represented as a four-dimensional normalized vector.
class Quaterniond
{
public:
    /// Construct an identity quaternion.
    Quaterniond() noexcept
       : w_(1.0),
         x_(0.0),
         y_(0.0),
         z_(0.0)
    {
    }

    /// Copy-construct from another quaternion.
    Quaterniond(const Quaterniond & quat ) noexcept
       :w_(quat.w_),
        x_(quat.x_),
        y_(quat.y_),
        z_(quat.z_)
    {
    }
    Quaterniond(const Quaternion & quat ) noexcept
       :w_(quat.w_),
        x_(quat.x_),
        y_(quat.y_),
        z_(quat.z_)
    {
    }

    /// Construct from values.
    Quaterniond(Float w, Float x, Float y, Float z) noexcept
       :w_(w),
        x_(x),
        y_(y),
        z_(z)
    {
    }

    /// Construct from a Float array.
    explicit Quaterniond(const Float* data) noexcept
       :w_(data[0]),
        x_(data[1]),
        y_(data[2]),
        z_(data[3])
    {
    }

    /// Construct from an angle (in degrees) and axis.
    Quaterniond(Float angle, const Vector3d& axis) noexcept
    {
        FromAngleAxis(angle, axis);
    }

    /// Construct from an angle (in degrees, for Urho2D).
    explicit Quaterniond(Float angle) noexcept
    {
        FromAngleAxis(angle, Vector3d::FORWARD);
    }

    /// Construct from Euler angles (in degrees.)
    Quaterniond(Float x, Float y, Float z) noexcept
    {
        FromEulerAngles(x, y, z);
    }

    /// Construct from the rotation difference between two direction vectors.
    Quaterniond(const Vector3d& start, const Vector3d& end) noexcept
    {
        FromRotationTo(start, end);
    }

    /// Construct from orthonormal axes.
    Quaterniond(const Vector3d& xAxis, const Vector3d& yAxis, const Vector3d& zAxis) noexcept
    {
        FromAxes(xAxis, yAxis, zAxis);
    }

    /// Construct from a rotation matrix.
    explicit Quaterniond(const Matrix3d & matrix ) noexcept
    {
        FromRotationMatrix( matrix );
    }

    /// Assign from another quaternion.
    Quaterniond& operator =(const Quaterniond& rhs) noexcept
    {
        w_ = rhs.w_;
        x_ = rhs.x_;
        y_ = rhs.y_;
        z_ = rhs.z_;

        return *this;
    }

    /// Add-assign a quaternion.
    Quaterniond& operator +=(const Quaterniond& rhs)
    {
        w_ += rhs.w_;
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;

        return *this;
    }

    /// Multiply-assign a scalar.
    Quaterniond& operator *=(Float rhs)
    {
        w_ *= rhs;
        x_ *= rhs;
        y_ *= rhs;
        z_ *= rhs;

        return *this;
    }

    /// Test for equality with another quaternion without epsilon.
    bool operator ==(const Quaterniond& rhs) const
    {
        return w_ == rhs.w_ && x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
    }

    /// Test for inequality with another quaternion without epsilon.
    bool operator !=(const Quaterniond& rhs) const { return !(*this == rhs); }

    /// Multiply with a scalar.
    Quaterniond operator *(Float rhs) const
    {
        return Quaterniond(w_ * rhs, x_ * rhs, y_ * rhs, z_ * rhs);
    }

    /// Return negation.
    Quaterniond operator -() const
    {
        return Quaterniond(-w_, -x_, -y_, -z_);
    }

    /// Add a quaternion.
    Quaterniond operator +(const Quaterniond& rhs) const
    {
        return Quaterniond(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
    }

    /// Subtract a quaternion.
    Quaterniond operator -(const Quaterniond& rhs) const
    {
        return Quaterniond(w_ - rhs.w_, x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
    }

    /// Multiply a quaternion.
    Quaterniond operator *(const Quaterniond& rhs) const
    {
        return Quaterniond(
            w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_,
            w_ * rhs.x_ + x_ * rhs.w_ + y_ * rhs.z_ - z_ * rhs.y_,
            w_ * rhs.y_ + y_ * rhs.w_ + z_ * rhs.x_ - x_ * rhs.z_,
            w_ * rhs.z_ + z_ * rhs.w_ + x_ * rhs.y_ - y_ * rhs.x_
        );
    }

    /// Multiply a Vecotr3d.
    Vector3d operator *(const Vector3d & rhs) const
    {
        Vector3d qVec(x_, y_, z_);
        Vector3d cross1(qVec.CrossProduct(rhs));
        Vector3d cross2(qVec.CrossProduct(cross1));

        return rhs + 2.0 * (cross1 * w_ + cross2);
    }

    /// Define from an angle (in degrees) and axis.
    void FromAngleAxis(Float angle, const Vector3d& axis);
    /// Define from Euler angles (in degrees.)
    void FromEulerAngles(Float x, Float y, Float z);
    /// Define from the rotation difference between two direction vectors.
    void FromRotationTo(const Vector3d& start, const Vector3d& end);
    /// Define from orthonormal axes.
    void FromAxes(const Vector3d& xAxis, const Vector3d& yAxis, const Vector3d& zAxis);
    /// Define from a rotation matrix.
    void FromRotationMatrix(const Matrix3d& matrix);
    /// Define from a direction to look in and an up direction. Return true if successful, or false if would result in a NaN, in which case the current value remains.
    bool FromLookRotation(const Vector3d& direction, const Vector3d& up = Vector3d::UP);

    /// Normalize to unit length.
    void Normalize()
    {
        Float lenSquared = LengthSquared();
        if (!Urho3D::Equals(lenSquared, 1.0) && lenSquared > 0.0)
        {
            Float invLen = 1.0 / sqrtf(lenSquared);
            w_ *= invLen;
            x_ *= invLen;
            y_ *= invLen;
            z_ *= invLen;
        }
    }

    /// Return normalized to unit length.
    Quaterniond Normalized() const
    {
        Float lenSquared = LengthSquared();
        if (!Urho3D::Equals(lenSquared, 1.0) && lenSquared > 0.0)
        {
            Float invLen = 1.0 / std::sqrt(lenSquared);
            return *this * invLen;
        }
        else
            return *this;
    }

    /// Return inverse.
    Quaterniond Inverse() const
    {
        Float lenSquared = LengthSquared();
        if (lenSquared == 1.0)
            return Conjugate();
        else if (lenSquared >= M_EPSILON)
            return Conjugate() * (1.0 / lenSquared);
        else
            return IDENTITY;
    }

    /// Return squared length.
    Float LengthSquared() const
    {
        return w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_;
    }

    /// Calculate dot product.
    Float DotProduct(const Quaterniond& rhs) const
    {
        return w_ * rhs.w_ + x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_;
    }

    /// Test for equality with another quaternion with epsilon.
    bool Equals(const Quaterniond& rhs) const
    {
        return Urho3D::Equals(w_, rhs.w_) && Urho3D::Equals(x_, rhs.x_) && Urho3D::Equals(y_, rhs.y_) && Urho3D::Equals(z_, rhs.z_);
    }

    /// Return whether is NaN.
    bool IsNaN() const { return Urho3D::IsNaN(w_) || Urho3D::IsNaN(x_) || Urho3D::IsNaN(y_) || Urho3D::IsNaN(z_); }

    /// Return conjugate.
    Quaterniond Conjugate() const
    {
        return Quaterniond(w_, -x_, -y_, -z_);
    }

    /// Return Euler angles in degrees.
    Vector3d EulerAngles() const;
    /// Return yaw angle in degrees.
    Float YawAngle() const;
    /// Return pitch angle in degrees.
    Float PitchAngle() const;
    /// Return roll angle in degrees.
    Float RollAngle() const;
    /// Return rotation axis.
    Vector3d Axis() const;
    /// Return rotation angle.
    Float Angle() const;
    /// Return the rotation matrix that corresponds to this quaternion.
    Matrix3d RotationMatrix() const;
    /// Spherical interpolation with another quaternion.
    Quaterniond Slerp(const Quaterniond& rhs, Float t) const;
    /// Normalized linear interpolation with another quaternion.
    Quaterniond Nlerp(const Quaterniond& rhs, Float t, bool shortestPath = false) const;

    /// Return float data.
    const Float* Data() const { return &w_; }

    /// Return as string.
    String ToString() const;

    /// W coordinate.
    Float w_;
    /// X coordinate.
    Float x_;
    /// Y coordinate.
    Float y_;
    /// Z coordinate.
    Float z_;

    /// Identity quaternion.
    static const Quaterniond IDENTITY;
};

}



#endif
