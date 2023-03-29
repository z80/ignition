
#include "bounding_node_gd.h"
#include "marching_cubes_dual_gd.h"

namespace Ign
{

void BoundingNodeGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("create_adjacent_node", "dx", "dy", "dz"), &BoundingNodeGd::create_adjacent_node, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("equals_to", "other_node"),                &BoundingNodeGd::equals_to,            Variant::BOOL );
	ClassDB::bind_method( D_METHOD("get_node_id"),                            &BoundingNodeGd::get_node_id,          Variant::STRING );
	ClassDB::bind_method( D_METHOD("get_adjacent_node_id", "dx", "dy", "dz"), &BoundingNodeGd::get_adjacent_node_id, Variant::STRING );
	ClassDB::bind_method( D_METHOD("get_center", "surface"),                  &BoundingNodeGd::get_center,           Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("get_size", "surface"),                    &BoundingNodeGd::get_size,             Variant::REAL );
	ClassDB::bind_method( D_METHOD("get_hash"),                               &BoundingNodeGd::get_hash,             Variant::STRING );
}

BoundingNodeGd::BoundingNodeGd()
	: Reference()
{
}

BoundingNodeGd::~BoundingNodeGd()
{
}

Ref<BoundingNodeGd> BoundingNodeGd::create_adjacent_node( int dx, int dy, int dz ) const
{
	Ref<BoundingNodeGd> ret;
	ret.instance();
	ret->node = node.create_adjacent_node( dx, dy, dz );

	return ret;
}

bool BoundingNodeGd::equals_to( const Ref<BoundingNodeGd> & other ) const
{
	const BoundingNodeGd * b = other.ptr();
	if (b == nullptr)
		return false;

	if ( node.at != b->node.at )
		return false;

	if ( node.size != b->node.size )
		return false;

	return true;
}

String BoundingNodeGd::get_node_id() const
{
	String ret = itos(node.at.x) + String(",") +
		         itos(node.at.y) + String(",") +
		         itos(node.at.z) + String(",") +
		         itos(node.size);
	return ret;
}

String BoundingNodeGd::get_adjacent_node_id( int dx, int dy, int dz ) const
{
	String ret = itos(node.at.x + dx*node.size) + String(",") +
                 itos(node.at.y + dy*node.size) + String(",") +
                 itos(node.at.z + dz*node.size) + String(",") +
                 itos(node.size);
	return ret;
}

Ref<Se3Ref> BoundingNodeGd::get_center( const Ref<MarchingCubesDualGd> & surface ) const
{
	const VectorInt c               = node.center();
	const MarchingCubesDual & cubes = surface->cubes;
	const Vector3d at               = cubes.at_in_source( c );
	const SE3 se3                   = cubes.se3_in_point( at );

	Ref<Se3Ref> ret_se3;
	ret_se3.instance();
	ret_se3->se3 = se3;
	return ret_se3;
}

real_t BoundingNodeGd::get_size( const Ref<MarchingCubesDualGd> & surface ) const
{
	const MarchingCubesDual & cubes = surface->cubes;

	const Float ret = node.node_size( &cubes );
	return ret;
}

String BoundingNodeGd::get_hash() const
{
	const uint64_t h = node.hash.state();
	const String s_hash = uitos( h );
	return s_hash;
}






}





