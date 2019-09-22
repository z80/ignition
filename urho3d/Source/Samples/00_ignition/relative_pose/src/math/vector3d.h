
#ifndef __VECTOR3D_H_
#define __VECTOR3D_H_

#include "data_types.h"
#include "Urho3D/Math/Vector3.h"

using namespace Urho3D;

namespace Ign
{

/// Three-dimensional vector.
class Vector3d
{
public:
    /// Construct a zero vector.
    Vector3d() noexcept :
        x_(0.0),
        y_(0.0),
        z_(0.0)
    {
    }

    /// Copy-construct from another vector.
    Vector3d(const Vector3d& vector) noexcept = default;
    Vector3d(const Vector3& vector)
        : x_( vector.x_ ),
          y_( vector.y_ ),
          z_( vector.z_ )
    {
    }

    /// Construct from a two-dimensional vector and the Z coordinate.
    Vector3d(const Vector2& vector, Float z) noexcept :
        x_(vector.x_),
        y_(vector.y_),
        z_(z)
    {
    }

    /// Construct from a two-dimensional vector (for Urho2D).
    explicit Vector3d( const Vector2 & vector ) noexcept :
        x_(vector.x_),
        y_(vector.y_),
        z_(0.0)
    {
    }

    /// Construct from an IntVector3.
    explicit Vector3d( const IntVector3 & vector ) noexcept :
        x_((Float)vector.x_),
        y_((Float)vector.y_),
        z_((Float)vector.z_)
    {
    }

    /// Construct from coordinates.
    Vector3d(Float x, Float y, Float z) noexcept :
        x_(x),
        y_(y),
        z_(z)
    {
    }

    /// Construct from two-dimensional coordinates (for Urho2D).
    Vector3d(Float x, Float y) noexcept :
        x_(x),
        y_(y),
        z_(0.0)
    {
    }

    /// Construct from a Float array.
    explicit Vector3d(const Float* data) noexcept :
        x_(data[0]),
        y_(data[1]),
        z_(data[2])
    {
    }

    Vector3 vector3() const
    {
        return Vector3( x_, y_, z_ );
    }

    /// Assign from another vector.
    Vector3d& operator =(const Vector3d& rhs) noexcept = default;

    Vector3d & operator =( const Vector3 & rhs )
    {
        x_ = rhs.x_;
        y_ = rhs.y_;
        z_ = rhs.z_;
        return *this;
    }

    /// Test for equality with another vector without epsilon.
    bool operator ==(const Vector3d& rhs) const { return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_; }

    /// Test for inequality with another vector without epsilon.
    bool operator !=(const Vector3d& rhs) const { return x_ != rhs.x_ || y_ != rhs.y_ || z_ != rhs.z_; }

    /// Add a vector.
    Vector3d operator +(const Vector3d& rhs) const { return Vector3d(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_); }

    /// Return negation.
    Vector3d operator -() const { return Vector3d(-x_, -y_, -z_); }

    /// Subtract a vector.
    Vector3d operator -(const Vector3d& rhs) const { return Vector3d(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_); }

    /// Multiply with a scalar.
    Vector3d operator *(Float rhs) const { return Vector3d(x_ * rhs, y_ * rhs, z_ * rhs); }

    /// Multiply with a vector.
    Vector3d operator *(const Vector3d& rhs) const { return Vector3d(x_ * rhs.x_, y_ * rhs.y_, z_ * rhs.z_); }

    /// Divide by a scalar.
    Vector3d operator /(Float rhs) const { return Vector3d(x_ / rhs, y_ / rhs, z_ / rhs); }

    /// Divide by a vector.
    Vector3d operator /(const Vector3d& rhs) const { return Vector3d(x_ / rhs.x_, y_ / rhs.y_, z_ / rhs.z_); }

    /// Add-assign a vector.
    Vector3d& operator +=(const Vector3d& rhs)
    {
        x_ += rhs.x_;
        y_ += rhs.y_;
        z_ += rhs.z_;
        return *this;
    }

    /// Subtract-assign a vector.
    Vector3d& operator -=(const Vector3d& rhs)
    {
        x_ -= rhs.x_;
        y_ -= rhs.y_;
        z_ -= rhs.z_;
        return *this;
    }

    /// Multiply-assign a scalar.
    Vector3d& operator *=(Float rhs)
    {
        x_ *= rhs;
        y_ *= rhs;
        z_ *= rhs;
        return *this;
    }

    /// Multiply-assign a vector.
    Vector3d& operator *=(const Vector3d& rhs)
    {
        x_ *= rhs.x_;
        y_ *= rhs.y_;
        z_ *= rhs.z_;
        return *this;
    }

    /// Divide-assign a scalar.
    Vector3d& operator /=(Float rhs)
    {
        Float invRhs = 1.0 / rhs;
        x_ *= invRhs;
        y_ *= invRhs;
        z_ *= invRhs;
        return *this;
    }

    /// Divide-assign a vector.
    Vector3d& operator /=(const Vector3d& rhs)
    {
        x_ /= rhs.x_;
        y_ /= rhs.y_;
        z_ /= rhs.z_;
        return *this;
    }

    /// Normalize to unit length.
    void Normalize()
    {
        Float lenSquared = LengthSquared();
        if (!Urho3D::Equals(lenSquared, 1.0) && lenSquared > 0.0)
        {
            Float invLen = 1.0 / sqrtf(lenSquared);
            x_ *= invLen;
            y_ *= invLen;
            z_ *= invLen;
        }
    }

    /// Return length.
    Float Length() const { return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_); }

    /// Return squared length.
    Float LengthSquared() const { return x_ * x_ + y_ * y_ + z_ * z_; }

    /// Calculate dot product.
    Float DotProduct(const Vector3d& rhs) const { return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_; }

    /// Calculate absolute dot product.
    Float AbsDotProduct(const Vector3d& rhs) const
    {
        return Urho3D::Abs(x_ * rhs.x_) + Urho3D::Abs(y_ * rhs.y_) + Urho3D::Abs(z_ * rhs.z_);
    }

    /// Project direction vector onto axis.
    Float ProjectOntoAxis(const Vector3d& axis) const { return DotProduct(axis.Normalized()); }

    /// Project position vector onto plane with given origin and normal.
    Vector3d ProjectOntoPlane(const Vector3d& origin, const Vector3d& normal) const
    {
        const Vector3d delta = *this - origin;
        return *this - normal.Normalized() * delta.ProjectOntoAxis(normal);
    }

    /// Project position vector onto line segment.
    Vector3d ProjectOntoLine(const Vector3d& from, const Vector3d& to, bool clamped = false) const
    {
        const Vector3d direction = to - from;
        const Float lengthSquared = direction.LengthSquared();
        Float factor = (*this - from).DotProduct(direction) / lengthSquared;

        if (clamped)
            factor = Clamp(factor, 0.0, 1.0);

        return from + direction * factor;
    }

    /// Calculate distance to another position vector.
    Float DistanceToPoint(const Vector3d& point) const { return (*this - point).Length(); }

    /// Calculate distance to the plane with given origin and normal.
    Float DistanceToPlane(const Vector3d& origin, const Vector3d& normal) const { return (*this - origin).ProjectOntoAxis(normal); }

    /// Make vector orthogonal to the axis.
    Vector3d Orthogonalize(const Vector3d& axis) const { return axis.CrossProduct(*this).CrossProduct(axis).Normalized(); }

    /// Calculate cross product.
    Vector3d CrossProduct(const Vector3d& rhs) const
    {
        return Vector3d(
            y_ * rhs.z_ - z_ * rhs.y_,
            z_ * rhs.x_ - x_ * rhs.z_,
            x_ * rhs.y_ - y_ * rhs.x_
        );
    }

    /// Return absolute vector.
    Vector3d Abs() const { return Vector3d(Urho3D::Abs(x_), Urho3D::Abs(y_), Urho3D::Abs(z_)); }

    /// Linear interpolation with another vector.
    Vector3d Lerp(const Vector3d& rhs, Float t) const { return *this * (1.0 - t) + rhs * t; }

    /// Test for equality with another vector with epsilon.
    bool Equals(const Vector3d& rhs) const
    {
        return Urho3D::Equals(x_, rhs.x_) && Urho3D::Equals(y_, rhs.y_) && Urho3D::Equals(z_, rhs.z_);
    }

    /// Returns the angle between this vector and another vector in degrees.
    Float Angle(const Vector3d& rhs) const { return Urho3D::Acos(DotProduct(rhs) / (Length() * rhs.Length())); }

    /// Return whether is NaN.
    bool IsNaN() const { return Urho3D::IsNaN(x_) || Urho3D::IsNaN(y_) || Urho3D::IsNaN(z_); }

    /// Return normalized to unit length.
    Vector3d Normalized() const
    {
        Float lenSquared = LengthSquared();
        if (!Urho3D::Equals(lenSquared, 1.0) && lenSquared > 0.0)
        {
            Float invLen = 1.0 / sqrtf(lenSquared);
            return *this * invLen;
        }
        else
            return *this;
    }

    /// Return Float data.
    const Float* Data() const { return &x_; }

    /// Return as string.
    String ToString() const;

    /// Return hash value for HashSet & HashMap.
    unsigned ToHash() const
    {
        unsigned hash = 37;
        hash = 37 * hash + FloatToRawIntBits(x_);
        hash = 37 * hash + FloatToRawIntBits(y_);
        hash = 37 * hash + FloatToRawIntBits(z_);

        return hash;
    }

    /// X coordinate.
    Float x_;
    /// Y coordinate.
    Float y_;
    /// Z coordinate.
    Float z_;

    /// Zero vector.
    static const Vector3d ZERO;
    /// (-1,0,0) vector.
    static const Vector3d LEFT;
    /// (1,0,0) vector.
    static const Vector3d RIGHT;
    /// (0,1,0) vector.
    static const Vector3d UP;
    /// (0,-1,0) vector.
    static const Vector3d DOWN;
    /// (0,0,1) vector.
    static const Vector3d FORWARD;
    /// (0,0,-1) vector.
    static const Vector3d BACK;
    /// (1,1,1) vector.
    static const Vector3d ONE;
};

/// Multiply Vector3d with a scalar.
inline Vector3d operator *(Float lhs, const Vector3d& rhs) { return rhs * lhs; }

/// Multiply IntVector3 with a scalar.
inline IntVector3 operator *(int lhs, const IntVector3& rhs) { return rhs * lhs; }

/// Per-component linear interpolation between two 3-vectors.
inline Vector3d VectorLerp(const Vector3d& lhs, const Vector3d& rhs, const Vector3d& t) { return lhs + (rhs - lhs) * t; }

/// Per-component min of two 3-vectors.
inline Vector3d VectorMin(const Vector3d& lhs, const Vector3d& rhs) { return Vector3d(Min(lhs.x_, rhs.x_), Min(lhs.y_, rhs.y_), Min(lhs.z_, rhs.z_)); }

/// Per-component max of two 3-vectors.
inline Vector3d VectorMax(const Vector3d& lhs, const Vector3d& rhs) { return Vector3d(Max(lhs.x_, rhs.x_), Max(lhs.y_, rhs.y_), Max(lhs.z_, rhs.z_)); }

/// Per-component floor of 3-vector.
inline Vector3d VectorFloor(const Vector3d& vec) { return Vector3d(Floor(vec.x_), Floor(vec.y_), Floor(vec.z_)); }

/// Per-component round of 3-vector.
inline Vector3d VectorRound(const Vector3d& vec) { return Vector3d(Round(vec.x_), Round(vec.y_), Round(vec.z_)); }

/// Per-component ceil of 3-vector.
inline Vector3d VectorCeil(const Vector3d& vec) { return Vector3d(Ceil(vec.x_), Ceil(vec.y_), Ceil(vec.z_)); }

/// Per-component floor of 3-vector. Returns IntVector3.
inline IntVector3 VectorFloorToInt(const Vector3d& vec) { return IntVector3(FloorToInt(vec.x_), FloorToInt(vec.y_), FloorToInt(vec.z_)); }

/// Per-component round of 3-vector. Returns IntVector3.
inline IntVector3 VectorRoundToInt(const Vector3d& vec) { return IntVector3(RoundToInt(vec.x_), RoundToInt(vec.y_), RoundToInt(vec.z_)); }

/// Per-component ceil of 3-vector. Returns IntVector3.
inline IntVector3 VectorCeilToInt(const Vector3d& vec) { return IntVector3(CeilToInt(vec.x_), CeilToInt(vec.y_), CeilToInt(vec.z_)); }

/// Per-component min of two 3-vectors.
inline IntVector3 VectorMin(const IntVector3& lhs, const IntVector3& rhs) { return IntVector3(Min(lhs.x_, rhs.x_), Min(lhs.y_, rhs.y_), Min(lhs.z_, rhs.z_)); }

/// Per-component max of two 3-vectors.
inline IntVector3 VectorMax(const IntVector3& lhs, const IntVector3& rhs) { return IntVector3(Max(lhs.x_, rhs.x_), Max(lhs.y_, rhs.y_), Max(lhs.z_, rhs.z_)); }

/// Return a random value from [0, 1) from 3-vector seed.
inline Float StableRandom(const Vector3d& seed) { return StableRandom(Vector2(StableRandom(Vector2(seed.x_, seed.y_)), seed.z_)); }


}



#endif


