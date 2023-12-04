
#include "subdivide_source_dist_ref.h"
#include "ref_frame_node.h"
#include "cube_sphere_node.h"


namespace Ign
{

void SubdivideSourceDistRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_min_level", "lvl"), &SubdivideSourceDistRef::set_min_level );
	ClassDB::bind_method( D_METHOD("get_min_level"),        &SubdivideSourceDistRef::get_min_level );

	ClassDB::bind_method( D_METHOD("set_min_size", "sz"), &SubdivideSourceDistRef::set_min_size );
	ClassDB::bind_method( D_METHOD("get_min_size"),       &SubdivideSourceDistRef::get_min_size );

	ClassDB::bind_method( D_METHOD("set_min_angle", "angle"), &SubdivideSourceDistRef::set_min_angle );
	ClassDB::bind_method( D_METHOD("get_min_angle"),          &SubdivideSourceDistRef::get_min_angle );

	ADD_PROPERTY( PropertyInfo( Variant::FLOAT,   "min_level" ),  "set_min_level",  "get_min_level" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT,   "min_size" ),   "set_min_size",   "get_min_size" );
	ADD_PROPERTY( PropertyInfo( Variant::FLOAT,   "min_angle" ),  "set_min_angle",  "get_min_angle" );
}

SubdivideSourceDistRef::SubdivideSourceDistRef()
    : SubdivideSourceRef()
{
	source = &subdivide_source_dist;
}

SubdivideSourceDistRef::~SubdivideSourceDistRef()
{
}

void SubdivideSourceDistRef::set_min_level( int lvl )
{
	subdivide_source_dist.min_level = lvl;
}

int SubdivideSourceDistRef::get_min_level() const
{
	return subdivide_source_dist.min_level;
}

void SubdivideSourceDistRef::set_min_size( real_t sz )
{
	subdivide_source_dist.min_size = sz;
}

real_t SubdivideSourceDistRef::get_min_size() const
{
	return subdivide_source_dist.min_size;
}

void SubdivideSourceDistRef::set_min_angle( real_t angle )
{
	subdivide_source_dist.min_angle = angle;
}

real_t SubdivideSourceDistRef::get_min_angle() const
{
	return subdivide_source_dist.min_angle;
}









}










