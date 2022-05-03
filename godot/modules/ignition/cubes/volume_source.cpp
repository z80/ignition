
#include "volume_source.h"

namespace Ign
{

VolumeSource::VolumeSource()
    : MarchingVolumeObject() 
{
}

VolumeSource::~VolumeSource()
{
}

Float VolumeSource::value_global( const Vector3d & at ) const
{
    const Vector3d local_at = se3_inverted * at;
    const Float v = value( local_at );
    return v;
}

Float VolumeSource::value( const Vector3d & at ) const
{
    const Float d = at.LengthSquared() - 5.0;
    return d;
}

void VolumeSource::set_inverted( bool en )
{
    inverted = en;
}

bool VolumeSource::get_inverted() const
{
    return inverted;
}




}













