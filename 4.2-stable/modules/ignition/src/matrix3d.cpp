

#include "matrix3d.h"

#include <cstdio>

//#include "../DebugNew.h"

using namespace Urho3D;

namespace Ign
{

const Matrix3d Matrix3d::ZERO(
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0);

const Matrix3d Matrix3d::IDENTITY;

Matrix3d Matrix3d::Inverse() const
{
    Float det = m00_ * m11_ * m22_ +
                m10_ * m21_ * m02_ +
                m20_ * m01_ * m12_ -
                m20_ * m11_ * m02_ -
                m10_ * m01_ * m22_ -
                m00_ * m21_ * m12_;

    Float invDet = 1.0 / det;

    return Matrix3d(
        (m11_ * m22_ - m21_ * m12_) * invDet,
        -(m01_ * m22_ - m21_ * m02_) * invDet,
        (m01_ * m12_ - m11_ * m02_) * invDet,
        -(m10_ * m22_ - m20_ * m12_) * invDet,
        (m00_ * m22_ - m20_ * m02_) * invDet,
        -(m00_ * m12_ - m10_ * m02_) * invDet,
        (m10_ * m21_ - m20_ * m11_) * invDet,
        -(m00_ * m21_ - m20_ * m01_) * invDet,
        (m00_ * m11_ - m10_ * m01_) * invDet
    );
}


String Matrix3d::ToString() const
{
    char tempBuffer[MATRIX_CONVERSION_BUFFER_LENGTH];
    sprintf(tempBuffer, "%g %g %g %g %g %g %g %g %g", m00_, m01_, m02_, m10_, m11_, m12_, m20_, m21_, m22_);
    return String(tempBuffer);
}


}
