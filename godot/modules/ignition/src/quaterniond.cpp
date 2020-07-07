
#include "quaterniond.h"

#include <cstdio>

#include "../DebugNew.h"

namespace Ign
{

const Quaterniond Quaterniond::IDENTITY;

void Quaterniond::FromAngleAxis(Float angle, const Vector3d& axis)
{
    Vector3d normAxis = axis.Normalized();
    angle *= M_DEGTORAD_2;
    Float sinAngle = sinf(angle);
    Float cosAngle = cosf(angle);

    w_ = cosAngle;
    x_ = normAxis.x_ * sinAngle;
    y_ = normAxis.y_ * sinAngle;
    z_ = normAxis.z_ * sinAngle;
}

void Quaterniond::FromEulerAngles(Float x, Float y, Float z)
{
    // Order of rotations: Z first, then X, then Y (mimics typical FPS camera with gimbal lock at top/bottom)
    x *= M_DEGTORAD_2;
    y *= M_DEGTORAD_2;
    z *= M_DEGTORAD_2;
    Float sinX = sinf(x);
    Float cosX = cosf(x);
    Float sinY = sinf(y);
    Float cosY = cosf(y);
    Float sinZ = sinf(z);
    Float cosZ = cosf(z);

    w_ = cosY * cosX * cosZ + sinY * sinX * sinZ;
    x_ = cosY * sinX * cosZ + sinY * cosX * sinZ;
    y_ = sinY * cosX * cosZ - cosY * sinX * sinZ;
    z_ = cosY * cosX * sinZ - sinY * sinX * cosZ;
}

void Quaterniond::FromRotationTo(const Vector3d& start, const Vector3d& end)
{
    Vector3d normStart = start.Normalized();
    Vector3d normEnd = end.Normalized();
    Float d = normStart.DotProduct(normEnd);

    if (d > -1.0 + M_EPSILON)
    {
        Vector3d c = normStart.CrossProduct(normEnd);
        Float s = sqrtf((1.0 + d) * 2.0);
        Float invS = 1.0 / s;

        x_ = c.x_ * invS;
        y_ = c.y_ * invS;
        z_ = c.z_ * invS;
        w_ = 0.5 * s;
    }
    else
    {
        Vector3d axis = Vector3d::RIGHT.CrossProduct(normStart);
        if (axis.Length() < M_EPSILON)
            axis = Vector3d::UP.CrossProduct(normStart);

        FromAngleAxis(180.0, axis);
    }
}

void Quaterniond::FromAxes(const Vector3d& xAxis, const Vector3d& yAxis, const Vector3d& zAxis)
{
    Matrix3d matrix(
        xAxis.x_, yAxis.x_, zAxis.x_,
        xAxis.y_, yAxis.y_, zAxis.y_,
        xAxis.z_, yAxis.z_, zAxis.z_
    );

    FromRotationMatrix(matrix);
}

void Quaterniond::FromRotationMatrix( const Matrix3d &matrix )
{
    Float t = matrix.m00_ + matrix.m11_ + matrix.m22_;

    if (t > 0.0)
    {
        Float invS = 0.5 / sqrtf(1.0 + t);

        x_ = (matrix.m21_ - matrix.m12_) * invS;
        y_ = (matrix.m02_ - matrix.m20_) * invS;
        z_ = (matrix.m10_ - matrix.m01_) * invS;
        w_ = 0.25 / invS;
    }
    else
    {
        if (matrix.m00_ > matrix.m11_ && matrix.m00_ > matrix.m22_)
        {
            Float invS = 0.5 / sqrtf(1.0 + matrix.m00_ - matrix.m11_ - matrix.m22_);

            x_ = 0.25 / invS;
            y_ = (matrix.m01_ + matrix.m10_) * invS;
            z_ = (matrix.m20_ + matrix.m02_) * invS;
            w_ = (matrix.m21_ - matrix.m12_) * invS;
        }
        else if (matrix.m11_ > matrix.m22_)
        {
            Float invS = 0.5 / sqrtf(1.0 + matrix.m11_ - matrix.m00_ - matrix.m22_);

            x_ = (matrix.m01_ + matrix.m10_) * invS;
            y_ = 0.25 / invS;
            z_ = (matrix.m12_ + matrix.m21_) * invS;
            w_ = (matrix.m02_ - matrix.m20_) * invS;
        }
        else
        {
            Float invS = 0.5 / sqrtf(1.0 + matrix.m22_ - matrix.m00_ - matrix.m11_);

            x_ = (matrix.m02_ + matrix.m20_) * invS;
            y_ = (matrix.m12_ + matrix.m21_) * invS;
            z_ = 0.25 / invS;
            w_ = (matrix.m10_ - matrix.m01_) * invS;
        }
    }
}

bool Quaterniond::FromLookRotation(const Vector3d& direction, const Vector3d& up)
{
    Quaterniond ret;
    Vector3d forward = direction.Normalized();

    Vector3d v = forward.CrossProduct(up);
    // If direction & up are parallel and crossproduct becomes zero, use FromRotationTo() fallback
    if (v.LengthSquared() >= M_EPSILON)
    {
        v.Normalize();
        Vector3d up = v.CrossProduct(forward);
        Vector3d right = up.CrossProduct(forward);
        ret.FromAxes(right, up, forward);
    }
    else
        ret.FromRotationTo(Vector3d::FORWARD, forward);

    if (!ret.IsNaN())
    {
        (*this) = ret;
        return true;
    }
    else
        return false;
}

Vector3d Quaterniond::EulerAngles() const
{
    // Derivation from http://www.geometrictools.com/Documentation/EulerAngles.pdf
    // Order of rotations: Z first, then X, then Y
    Float check = 2.0 * (-y_ * z_ + w_ * x_);

    if (check < -0.995)
    {
        return Vector3d(
            -90.0,
            0.0,
            -atan2f(2.0 * (x_ * z_ - w_ * y_), 1.0 - 2.0 * (y_ * y_ + z_ * z_)) * M_RADTODEG
        );
    }
    else if (check > 0.995)
    {
        return Vector3d(
            90.0,
            0.0,
            atan2f(2.0 * (x_ * z_ - w_ * y_), 1.0 - 2.0 * (y_ * y_ + z_ * z_)) * M_RADTODEG
        );
    }
    else
    {
        return Vector3d(
            asinf(check) * M_RADTODEG,
            atan2f(2.0 * (x_ * z_ + w_ * y_), 1.0 - 2.0 * (x_ * x_ + y_ * y_)) * M_RADTODEG,
            atan2f(2.0 * (x_ * y_ + w_ * z_), 1.0 - 2.0 * (x_ * x_ + z_ * z_)) * M_RADTODEG
        );
    }
}

Float Quaterniond::YawAngle() const
{
    return EulerAngles().y_;
}

Float Quaterniond::PitchAngle() const
{
    return EulerAngles().x_;
}

Float Quaterniond::RollAngle() const
{
    return EulerAngles().z_;
}

Vector3d Quaterniond::Axis() const
{
    return Vector3d(x_, y_, z_) / sqrt(1.0 - w_ * w_);
}

Float Quaterniond::Angle() const
{
    return 2 * Acos(w_);
}

Matrix3d Quaterniond::RotationMatrix() const
{
    return Matrix3d(
        1.0 - 2.0 * y_ * y_ - 2.0 * z_ * z_,
        2.0 * x_ * y_ - 2.0 * w_ * z_,
        2.0 * x_ * z_ + 2.0 * w_ * y_,
        2.0 * x_ * y_ + 2.0 * w_ * z_,
        1.0 - 2.0 * x_ * x_ - 2.0 * z_ * z_,
        2.0 * y_ * z_ - 2.0 * w_ * x_,
        2.0 * x_ * z_ - 2.0 * w_ * y_,
        2.0 * y_ * z_ + 2.0 * w_ * x_,
        1.0 - 2.0 * x_ * x_ - 2.0 * y_ * y_
    );
}

Quaterniond Quaterniond::Slerp(const Quaterniond& rhs, Float t) const
{
    // Use fast approximation for Emscripten builds
#ifdef __EMSCRIPTEN__
    Float angle = DotProduct(rhs);
    Float sign = 1.f; // Multiply by a sign of +/-1 to guarantee we rotate the shorter arc.
    if (angle < 0.f)
    {
        angle = -angle;
        sign = -1.f;
    }

    Float a;
    Float b;
    if (angle < 0.999f) // perform spherical linear interpolation.
    {
        // angle = acos(angle); // After this, angle is in the range pi/2 -> 0 as the original angle variable ranged from 0 -> 1.
        angle = (-0.69813170079773212f * angle * angle - 0.87266462599716477f) * angle + 1.5707963267948966f;
        Float ta = t*angle;
        // Manually compute the two sines by using a very rough approximation.
        Float ta2 = ta*ta;
        b = ((5.64311797634681035370e-03f * ta2 - 1.55271410633428644799e-01f) * ta2 + 9.87862135574673806965e-01f) * ta;
        a = angle - ta;
        Float a2 = a*a;
        a = ((5.64311797634681035370e-03f * a2 - 1.55271410633428644799e-01f) * a2 + 9.87862135574673806965e-01f) * a;
    }
    else // If angle is close to taking the denominator to zero, resort to linear interpolation (and normalization).
    {
        a = 1.f - t;
        b = t;
    }
    // Lerp and renormalize.
    return (*this * (a * sign) + rhs * b).Normalized();
#else
    // Favor accuracy for native code builds
    Float cosAngle = DotProduct(rhs);
    Float sign = 1.0;
    // Enable shortest path rotation
    if (cosAngle < 0.0)
    {
        cosAngle = -cosAngle;
        sign = -1.0;
    }

    Float angle = acosf(cosAngle);
    Float sinAngle = sinf(angle);
    Float t1, t2;

    if (sinAngle > 0.001)
    {
        Float invSinAngle = 1.0 / sinAngle;
        t1 = sinf((1.0 - t) * angle) * invSinAngle;
        t2 = sinf(t * angle) * invSinAngle;
    }
    else
    {
        t1 = 1.0 - t;
        t2 = t;
    }

    return *this * t1 + (rhs * sign) * t2;
#endif
}

Quaterniond Quaterniond::Nlerp(const Quaterniond& rhs, Float t, bool shortestPath) const
{
    Quaterniond result;
    Float fCos = DotProduct(rhs);
    if (fCos < 0.0 && shortestPath)
        result = (*this) + (((-rhs) - (*this)) * t);
    else
        result = (*this) + ((rhs - (*this)) * t);
    result.Normalize();
    return result;
}

String Quaterniond::ToString() const
{
    char tempBuffer[CONVERSION_BUFFER_LENGTH];
    sprintf(tempBuffer, "%g %g %g %g", w_, x_, y_, z_);
    return String(tempBuffer);
}

}
