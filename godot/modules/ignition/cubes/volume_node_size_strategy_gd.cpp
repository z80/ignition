
#include "volume_node_size_strategy_gd.h"

namespace Ign
{


void VolumeNodeSizeStrategyGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_radius", "r"), &VolumeNodeSizeStrategyGd::set_radius );
	ClassDB::bind_method( D_METHOD("get_radius"),      &VolumeNodeSizeStrategyGd::get_radius, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_focal_point", "f"), &VolumeNodeSizeStrategyGd::set_focal_point );
	ClassDB::bind_method( D_METHOD("get_focal_point"),      &VolumeNodeSizeStrategyGd::get_focal_point, Variant::VECTOR3 );

	ClassDB::bind_method( D_METHOD("set_max_level", "level"), &VolumeNodeSizeStrategyGd::set_max_level );
	ClassDB::bind_method( D_METHOD("get_max_level"),          &VolumeNodeSizeStrategyGd::get_max_level, Variant::INT );

	ClassDB::bind_method( D_METHOD("compute_max_level", "detail_size"), &VolumeNodeSizeStrategyGd::compute_max_level, Variant::INT );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL,    "radius" ),        "set_radius",        "get_radius" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "focal_point" ),   "set_focal_point",   "get_focal_point" );
	ADD_PROPERTY( PropertyInfo( Variant::INT,     "max_level" ),     "set_max_level",     "get_max_level" );
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

void VolumeNodeSizeStrategyGd::set_max_level( int level )
{
	strategy.set_max_level( level );
}

int VolumeNodeSizeStrategyGd::get_max_level() const
{
	const int ret = strategy.get_max_level();
	return ret;
}

int VolumeNodeSizeStrategyGd::compute_max_level( real_t min_detail_size )
{
	const int ret = strategy.compute_max_level( min_detail_size );
	return ret;
}



}

