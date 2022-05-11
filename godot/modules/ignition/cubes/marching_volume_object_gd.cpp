
#include "marching_volume_object_gd.h"

namespace Ign
{

void MarchingVolumeObjectGd::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("set_se3", "se3"), &MarchingVolumeObjectGd::set_se3 );
    ClassDB::bind_method( D_METHOD("get_se3"),        &MarchingVolumeObjectGd::get_se3, Variant::OBJECT );

    ClassDB::bind_method( D_METHOD("at"),             &MarchingVolumeObjectGd::at,      Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD("set_bounding_radius", "r"), &MarchingVolumeObjectGd::set_bounding_radius );
    ClassDB::bind_method( D_METHOD("get_bounding_radius"),      &MarchingVolumeObjectGd::get_bounding_radius, Variant::REAL );
    
    ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "se3" ), "set_se3", "get_se3" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "bounding_radius" ), "set_bounding_radius", "get_bounding_radius" );
}

MarchingVolumeObjectGd::MarchingVolumeObjectGd()
	: Reference()
{
    object = nullptr;
}

MarchingVolumeObjectGd::~MarchingVolumeObjectGd()
{
}

void MarchingVolumeObjectGd::set_se3( const Ref<Se3Ref> & se3 )
{
	if ( object == nullptr )
		return;

    object->set_se3( se3.ptr()->se3 );
}

Ref<Se3Ref> MarchingVolumeObjectGd::get_se3() const
{
	Ref<Se3Ref> ret;
    ret.instance();

	if ( object == nullptr )
		return ret;

	ret.ptr()->se3 = object->get_se3();
    return ret;
}

Vector3 MarchingVolumeObjectGd::at() const
{
	if ( object == nullptr )
		return Vector3();

	const Vector3d v = object->at();
    const Vector3 ret( v.x_, v.y_, v.z_ );
    return ret;
}

void MarchingVolumeObjectGd::set_bounding_radius( real_t r )
{
	if ( object == nullptr )
		return;

	object->set_bounding_radius( r );
}

real_t MarchingVolumeObjectGd::get_bounding_radius() const
{
	if ( object == nullptr )
		return -1.0;

    const real_t ret = object->get_bounding_radius();
    return ret;
}






}











