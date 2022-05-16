
#include "volume_source.h"

namespace Ign
{

VolumeSource::VolumeSource()
    : MarchingVolumeObject() 
{
	inverted = false;
}

VolumeSource::~VolumeSource()
{
}

Float VolumeSource::value_global( const Vector3d & at )
{
    const Vector3d local_at = se3_inverted * at;
    const Float v = value( local_at );
    return v;
}

Float VolumeSource::value( const Vector3d & at )
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

int VolumeSource::material_global( const Vector3d & at )
{
	const Vector3d local_at = se3_inverted * at;
	const int m = material( local_at );
	return m;
}

int VolumeSource::material( const Vector3d & at )
{
	return 0;
}

void VolumeSource::set_material_only( bool only )
{
	material_only = only;
}

bool VolumeSource::get_material_only() const
{
	return material_only;
}

void VolumeSource::set_weak_material( bool weak )
{
	weak_material = weak;
}

bool VolumeSource::get_weak_material() const
{
	return weak_material;
}





}













