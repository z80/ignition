
#include "volume_source.h"

namespace Ign
{

VolumeSource::VolumeSource()
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

void VolumeSource::set_se3( const SE3 & new_se3 )
{
    se3 = new_se3;
	se3_inverted = se3.inverse();
}

const SE3 & VolumeSource::get_se3() const
{
    return se3;
}

void VolumeSource::set_inverted( bool en )
{
    inverted = en;
}

bool VolumeSource::get_inverted() const
{
    return inverted;
}


Float VolumeSource::max_node_size() const
{
    return 1.0;
}

Float VolumeSource::min_node_size() const
{
    return 1.0;
}

Float VolumeSource::max_node_size_at( const Vector3d & at ) const
{
    return 1.0;
}



}













