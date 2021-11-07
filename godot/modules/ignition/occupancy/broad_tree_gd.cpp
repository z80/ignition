
#include "broad_tree_gd.h"
#include "octree_mesh_gd.h"
#include "ref_frame_node.h"

namespace Ign
{

void BroadTreeGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_max_depth", "new_level"), &BroadTreeGd::set_max_depth );
	ClassDB::bind_method( D_METHOD("get_max_depth"),              &BroadTreeGd::get_max_depth, Variant::INT );
	ClassDB::bind_method( D_METHOD("clear"),                      &BroadTreeGd::clear );
	ClassDB::bind_method( D_METHOD("subdivide", "ref_frame_physics"), &BroadTreeGd::subdivide, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("get_octree_meshes_qty"),      &BroadTreeGd::get_octree_meshes_qty, Variant::INT );
	ClassDB::bind_method( D_METHOD("get_octree_mesh", "ind"),     &BroadTreeGd::get_octree_mesh, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("intersects_segment", "start", "end", "exclude_mesh"), &BroadTreeGd::intersects_segment, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("intersects_segment_face", "start", "end", "exclude_mesh"), &BroadTreeGd::intersects_segment_face, Variant::ARRAY );

	ADD_PROPERTY( PropertyInfo( Variant::INT, "max_depth" ), "set_max_depth", "get_max_depth" );
}

BroadTreeGd::BroadTreeGd()
	: Reference()
{
}

BroadTreeGd::~BroadTreeGd()
{
}

void BroadTreeGd::set_max_depth( int new_level )
{
	_broad_tree.set_max_depth( new_level );
}

int  BroadTreeGd::get_max_depth() const
{
	const int ret = _broad_tree.get_max_depth();
	return ret;
}

void BroadTreeGd::clear()
{
	_broad_tree.clear();
}

bool BroadTreeGd::subdivide( Node * ref_frame_physics )
{
	RefFrameNode * rf = Node::cast_to<RefFrameNode>( ref_frame_physics );
	if ( rf == nullptr )
		return false;

	_broad_tree.subdivide( rf );

	return true;
}

int BroadTreeGd::get_octree_meshes_qty() const
{
	const int ret = _broad_tree.get_octree_meshes_qty();
	return ret;
}

Node * BroadTreeGd::get_octree_mesh( int ind )
{
	OctreeMeshGd * octree_mesh = _broad_tree.get_octree_mesh( ind );
	Node * ret = Node::cast_to<Node>(octree_mesh);
	return ret;
}

bool BroadTreeGd::intersects_segment( const Vector3 & start, const Vector3 & end, Node * exclude_mesh ) const
{
	OctreeMeshGd * mesh = Node::cast_to<OctreeMeshGd>( exclude_mesh );
	const bool ret = _broad_tree.intersects_segment( start, end, mesh );
	return ret;
}

Array BroadTreeGd::intersects_segment_face( const Vector3 & start, const Vector3 & end, Node * exclude_mesh ) const
{
	real_t  dist;
	Vector3 at;
	OctreeMesh::FaceProperties props;
	OctreeMeshGd * mesh = Node::cast_to<OctreeMeshGd>( exclude_mesh );
	const bool ok = _broad_tree.intersects_segment_face( start, end, dist, at, props, mesh );
	Array ret;
	ret.push_back( ok );
	if ( ok )
	{
		ret.push_back( dist );
		ret.push_back( at );
		ret.push_back( props.position );
		ret.push_back( props.normal );
		ret.push_back( props.area );
	}
	return ret;
}


}




