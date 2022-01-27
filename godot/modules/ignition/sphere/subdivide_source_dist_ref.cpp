
#include "subdivide_source_dist_ref.h"
#include "ref_frame_node.h"
#include "cube_sphere_node.h"


namespace Ign
{

void SubdivideSourceDistRef::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_min_size", "sz"), &SubdivideSourceDistRef::set_min_size );
	ClassDB::bind_method( D_METHOD("get_min_size"),       &SubdivideSourceDistRef::get_min_size, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_min_angle", "angle"), &SubdivideSourceDistRef::set_min_angle );
	ClassDB::bind_method( D_METHOD("get_min_angle"),          &SubdivideSourceDistRef::get_min_angle, Variant::REAL );

	ADD_PROPERTY( PropertyInfo( Variant::REAL,   "min_size" ),  "set_min_size",  "get_min_size" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL,   "min_angle" ), "set_min_angle", "get_min_angle" );
}

SubdivideSourceDistRef::SubdivideSourceDistRef()
    : SubdivideSourceRef()
{
	source = &subdivide_source_dist;
}

SubdivideSourceDistRef::~SubdivideSourceDistRef()
{
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










