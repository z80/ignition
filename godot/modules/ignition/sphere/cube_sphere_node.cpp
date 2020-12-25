
#include "cube_sphere_node.h"
#include "height_source_ref.h"

namespace Ign
{

void CubeSphereNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_height_source", "height_source"), &CubeSphereNode::set_height_source );

	ClassDB::bind_method( D_METHOD("set_r", "r"), &CubeSphereNode::set_r );
	ClassDB::bind_method( D_METHOD("get_r" ),     &CubeSphereNode::get_r, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_h", "h"), &CubeSphereNode::set_h );
	ClassDB::bind_method( D_METHOD("get_h" ),     &CubeSphereNode::get_h, Variant::REAL );

}


CubeSphereNode::CubeSphereNode()
	: MeshInstance()
{
	height_source = nullptr;

	check_period = 1.0;
}

CubeSphereNode::~CubeSphereNode()
{
	if (height_source)
		height_source->unreference();
}

void CubeSphereNode::set_height_source( Reference * hs )
{
	if (height_source)
		height_source->unreference();
	height_source = Object::cast_to<HeightSourceRef>( hs );
	height_source->reference();
}


void CubeSphereNode::set_r( real_t r )
{
	sphere.set_r( r );
}

real_t CubeSphereNode::get_r() const
{
	return sphere.r();
}

void CubeSphereNode::set_h( real_t h )
{
	sphere.set_h( h );
}

real_t CubeSphereNode::get_h() const
{
	return sphere.h();
}

void CubeSphereNode::clear_levels()
{
	subdivide_source.clear_levels();
}

void CubeSphereNode::add_level( real_t sz, real_t dist )
{
	subdivide_source.add_level( sz, dist );
}

void CubeSphereNode::set_subdivision_check_period( real_t sec )
{
	check_period = sec;
}

real_t CubeSphereNode::get_subdivision_check_period() const
{
	return check_period;
}

void CubeSphereNode::clear_points_of_interest()
{
	points_of_interest.clear();
}

void CubeSphereNode::add_point_of_interest( const Vector3 & at, bool close )
{
	SubdivideSource::SubdividePoint pt;
	pt.at = Vector3d( at.x, at.y, at.z );
	pt.close = close;
	points_of_interest.push_back( pt );
}

bool CubeSphereNode::need_rebuild()
{
	const bool rebuild_is_needed = subdivide_source.need_subdivide( &sphere, points_of_interest );
	return rebuild_is_needed;
}

void CubeSphereNode::rebuild()
{
	sphere.subdivide( &subdivide_source );
	sphere.apply_source( height_source );
	sphere.triangle_list( all_tris );

	// Fill in arrays.
	const int qty = all_tris.size();
	for ( int i=0; i<qty; i++ )
	{
		
	}

	Array arrays;
	arrays.resize( ArrayMesh::ARRAY_MAX );
	arrays[ArrayMesh::ARRAY_VERTEX]  = vertices;
	arrays[ArrayMesh::ARRAY_NORMAL]  = normals;
	arrays[ArrayMesh::ARRAY_TANGENT] = tangents;
	arrays[ArrayMesh::ARRAY_COLOR]   = colors;
	arrays[ArrayMesh::ARRAY_TEX_UV]  = uvs;
	arrays[ArrayMesh::ARRAY_TEX_UV2] = uvs;

	Ref<ArrayMesh> am = memnew(ArrayMesh);
	am->add_surface_from_arrays( Mesh::PRIMITIVE_TRIANGLES, arrays );

	this->set_mesh( am );
}

PoolVector3Array CubeSphereNode::triangles()
{
	PoolVector3Array arr;
	return arr;
}

PoolVector3Array CubeSphereNode::collision_triangles( real_t dist )
{
	PoolVector3Array arr;
	return arr;
}



}






