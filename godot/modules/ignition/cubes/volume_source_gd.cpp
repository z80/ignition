
#include "volume_source_gd.h"

namespace Ign
{

void VolumeSourceGd::_bind_methods()
{
    ClassDB::bind_method( D_METHOD("value", "at"), &VolumeSourceGd::value, Variant::REAL );

    ClassDB::bind_method( D_METHOD("set_inverted", "en"), &VolumeSourceGd::set_inverted );
    ClassDB::bind_method( D_METHOD("get_inverted"),       &VolumeSourceGd::get_inverted, Variant::BOOL );

    ClassDB::bind_method( D_METHOD("max_node_size"),      &VolumeSourceGd::max_node_size, Variant::REAL );
    ClassDB::bind_method( D_METHOD("min_node_size"),      &VolumeSourceGd::min_node_size, Variant::REAL );

    ClassDB::bind_method( D_METHOD("max_node_size_local", "at"), &VolumeSourceGd::max_node_size_local, Variant::REAL );
	ClassDB::bind_method( D_METHOD("min_node_size_local", "at"), &VolumeSourceGd::min_node_size_local, Variant::REAL );

	ClassDB::bind_method( D_METHOD("material_global", "at"), &VolumeSourceGd::material_global, Variant::INT );
	ClassDB::bind_method( D_METHOD("material", "at"),        &VolumeSourceGd::material,        Variant::INT );

	ClassDB::bind_method( D_METHOD("priority"),              &VolumeSourceGd::priority,        Variant::INT );

	ADD_PROPERTY( PropertyInfo( Variant::BOOL, "inverted" ),      "set_inverted",      "get_inverted" );
}

VolumeSourceGd::VolumeSourceGd()
    : MarchingVolumeObjectGd()
{
    source = nullptr;
	last_priority = -1;
}

VolumeSourceGd::~VolumeSourceGd()
{
}

real_t VolumeSourceGd::value( const Vector3 & at ) const
{
	if ( source == nullptr )
		return -1.0;

    const Float ret = source->value( Vector3d( at.x, at.y, at.z ) );
    return ret;
}

void VolumeSourceGd::set_inverted( bool en )
{
	if ( source == nullptr )
		return;

	source->set_inverted( en );
}

bool VolumeSourceGd::get_inverted() const
{
	if ( source == nullptr )
		return false;

	const bool ret = source->get_inverted();
    return ret;
}

real_t VolumeSourceGd::max_node_size() const
{
	if ( source == nullptr )
		return -1.0;

	const Float ret = source->max_node_size();
    return ret;
}

real_t VolumeSourceGd::min_node_size() const
{
	if ( source == nullptr )
		return -1.0;

	const Float ret = source->min_node_size();
    return ret;
}

real_t VolumeSourceGd::max_node_size_local( const Vector3 & at ) const
{
	if ( source == nullptr )
		return -1.0;

	const Float ret = source->max_node_size_local( Vector3d( at.x, at.y, at.z ) );
    return ret;
}

real_t VolumeSourceGd::min_node_size_local( const Vector3 & at ) const
{
	if ( source == nullptr )
		return -1.0;

	const Float ret = source->min_node_size_local( Vector3d( at.x, at.y, at.z ) );
	return ret;
}

int VolumeSourceGd::material_global( const Vector3 & at )
{
	if ( source == nullptr )
		return -1;

	const int ret = source->material_global( at, &last_priority );
	return ret;
}

int VolumeSourceGd::material( const Vector3 & at )
{
	if ( source == nullptr )
		return -1;

	const int ret = source->material( at, &last_priority );
	return ret;
}

int VolumeSourceGd::priority() const
{
	const int ret = last_priority;
	return ret;
}




}

