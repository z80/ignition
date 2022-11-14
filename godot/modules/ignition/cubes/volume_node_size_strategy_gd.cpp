
#include "volume_node_size_strategy_gd.h"

namespace Ign
{


void VolumeNodeSizeStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_radius", "r"), &VolumeNodeSizeStrategyGd::set_radius );
	ClassDB::bind_method( D_METHOD("get_radius"),      &VolumeNodeSizeStrategyGd::get_radius, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_focal_point", "f"), &VolumeNodeSizeStrategyGd::set_focal_point );
	ClassDB::bind_method( D_METHOD("get_focal_point"),      &VolumeNodeSizeStrategyGd::get_focal_point, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_height", "h"), &VolumeNodeSizeStrategyGd::set_height );
	ClassDB::bind_method( D_METHOD("get_height"),      &VolumeNodeSizeStrategyGd::get_height, Variant::REAL );

	ADD_PROPERTY( PropertyInfo( Variant::REAL,    "radius" ),        "set_radius",        "get_radius" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "focal_point" ),   "set_focal_point",   "get_focal_point" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL,    "height" ),        "set_height",        "get_height" );
}

VolumeNodeSizeStrategyGd::VolumeNodeSizeStrategyGd()
{
}

VolumeNodeSizeStrategyGd::~VolumeNodeSizeStrategyGd()
{
}

void VolumeNodeSizeStrategyGd::set_radius( real_t r )
{
	strategy.set_radius( r );
}

real_t VolumeNodeSizeStrategyGd::get_radius() const
{
	const real_t ret = strategy.get_radius();
	return ret;
}

void VolumeNodeSizeStrategyGd::set_focal_point( const Vector3 & r )
{
	const Vector3d arg( r.x, r.y, r.z );
	strategy.set_focal_point( arg );
}

Vector3 VolumeNodeSizeStrategyGd::get_focal_point() const
{
	const Vector3d r = strategy.get_focal_point();
	const Vector3 ret( r.x_, r.y_, r.z_ );
	return ret;
}

void VolumeNodeSizeStrategyGd::set_height( real_t h )
{
	strategy.set_height( h );
}

real_t VolumeNodeSizeStrategyGd::get_height() const
{
	const real_t ret = strategy.get_height();
	return ret;
}



}

