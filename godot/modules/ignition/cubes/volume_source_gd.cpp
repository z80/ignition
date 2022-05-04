
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

    //ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "se3" ), "set_se3", "get_se3" );
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
    const Float ret = source->value( Vector3d( at.x, at.y, at.z ) );
    return ret;
}

void VolumeSourceGd::set_inverted( bool en )
{
    source->set_inverted( en );
}

bool VolumeSourceGd::get_inverted() const
{
    const bool ret = source->get_inverted();
    return ret;
}

real_t VolumeSourceGd::max_node_size() const
{
    const Float ret = source->max_node_size();
    return ret;
}

real_t VolumeSourceGd::min_node_size() const
{
    const Float ret = source->min_node_size();
    return ret;
}

real_t VolumeSourceGd::max_node_size_at( const Vector3 & at ) const
{
    const Float ret = source->max_node_size_at( Vector3d( at.x, at.y, at.z ) );
    return ret;
}



}

