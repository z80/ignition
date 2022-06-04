
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

    ClassDB::bind_method( D_METHOD("max_node_size_at", "at"), &VolumeSourceGd::max_node_size_at, Variant::REAL );


	ClassDB::bind_method( D_METHOD("material", "at"), &VolumeSourceGd::material, Variant::INT );

	ClassDB::bind_method( D_METHOD("set_material_only", "only"), &VolumeSourceGd::set_material_only );
	ClassDB::bind_method( D_METHOD("get_material_only"),         &VolumeSourceGd::get_material_only, Variant::BOOL );


	ADD_PROPERTY( PropertyInfo( Variant::BOOL, "inverted" ),      "set_inverted",      "get_inverted" );
	ADD_PROPERTY( PropertyInfo( Variant::BOOL, "material_only" ), "set_material_only", "get_material_only" );
}

VolumeSourceGd::VolumeSourceGd()
    : MarchingVolumeObjectGd()
{
    source = nullptr;
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

real_t VolumeSourceGd::max_node_size_at( const Vector3 & at ) const
{
	if ( source == nullptr )
		return -1.0;

	const Float ret = source->max_node_size_at( Vector3d( at.x, at.y, at.z ) );
    return ret;
}

int VolumeSourceGd::material( const Vector3 & at )
{
	if ( source == nullptr )
		return -1;

	const int ret = source->material( at );
	return ret;
}

void VolumeSourceGd::set_material_only( bool only )
{
	if ( source == nullptr )
		return;

	source->set_material_only( only );
}

bool VolumeSourceGd::get_material_only() const
{
	if ( source == nullptr )
		return false;

	const bool ret = source->get_material_only();
	return ret;
}






}

