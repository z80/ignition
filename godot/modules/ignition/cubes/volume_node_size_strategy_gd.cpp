
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

	ClassDB::bind_method( D_METHOD("set_max_distance", "dist"), &VolumeNodeSizeStrategyGd::set_max_distance );
	ClassDB::bind_method( D_METHOD("get_max_distance"),         &VolumeNodeSizeStrategyGd::get_max_distance, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_max_node_size", "dist"), &VolumeNodeSizeStrategyGd::set_max_node_size );
	ClassDB::bind_method( D_METHOD("get_max_node_size"),         &VolumeNodeSizeStrategyGd::get_max_node_size, Variant::REAL );

	ClassDB::bind_method( D_METHOD("local_node_size", "at", "sz"), &VolumeNodeSizeStrategyGd::local_node_size, Variant::REAL );

	ADD_PROPERTY( PropertyInfo( Variant::REAL,    "radius" ),        "set_radius",        "get_radius" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "focal_point" ),   "set_focal_point",   "get_focal_point" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL,    "height" ),        "set_height",        "get_height" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL,    "max_distance" ),  "set_max_distance",  "get_max_distance" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL,    "max_node_size" ), "set_max_node_size", "get_max_node_size" );
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

void VolumeNodeSizeStrategyGd::set_max_distance( real_t dist )
{
	strategy.set_max_distance( dist );
}

real_t VolumeNodeSizeStrategyGd::get_max_distance() const
{
	const real_t dist = strategy.get_max_distance();
	return dist;
}

void VolumeNodeSizeStrategyGd::set_max_node_size( real_t sz )
{
	strategy.set_max_node_size( sz );
}

real_t VolumeNodeSizeStrategyGd::get_max_node_size() const
{
	const real_t sz = strategy.get_max_node_size();
	return sz;
}

real_t VolumeNodeSizeStrategyGd::local_node_size( const Vector3 & node_at, const real_t node_size ) const
{
	const Vector3d node_at_d( node_at.x, node_at.y, node_at.z );
	const real_t ret = strategy.local_node_size( node_at_d, node_size );
	return ret;
}


}

