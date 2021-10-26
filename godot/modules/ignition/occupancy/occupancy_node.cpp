
#include "occupancy_node.h"

void OccupancyNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_node_size", "real_t"), &OccupancyNode::set_node_size, Variant::NIL );
	ClassDB::bind_method( D_METHOD("node_size"),               &OccupancyNode::node_size, Variant::REAL );
	ClassDB::bind_method( D_METHOD("rebuild"),                 &OccupancyNode::rebuild, Variant::NIL );
	ClassDB::bind_method( D_METHOD("lines"),                   &OccupancyNode::lines, Variant::POOL_VECTOR3_ARRAY );
	ClassDB::bind_method( D_METHOD("occupied", "vector3"),     &OccupancyNode::occupied, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("point_ajacent", "vector3"), &OccupancyNode::point_ajacent, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("intersects", "node"),      &OccupancyNode::intersects, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("touches", "node"),         &OccupancyNode::touches, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("touch_point", "node"),     &OccupancyNode::touch_point, Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("intersects_ray", "from", "to"), &OccupancyNode::intersects_ray, Variant::BOOL );
}


OccupancyNode::OccupancyNode()
	: Node()
{
}

OccupancyNode::~OccupancyNode()
{
}

void OccupancyNode::set_node_size( real_t sz )
{
	_tree.set_node_size( sz );
}

real_t OccupancyNode::node_size() const
{
	return _tree.node_size();
}

void OccupancyNode::rebuild()
{
	_tree.clear();
	Node * p = get_parent();
	if ( !p )
		return;
	MeshInstance * mi = Object::cast_to<MeshInstance>( p );
	if ( !mi )
		return;

	const Transform t = mi->get_transform();
	const Ref<Mesh> m = mi->get_mesh();
	_tree.append( t, m );
	_tree.subdivide();
}

PoolVector<Vector3> OccupancyNode::lines()
{
	return _tree.lines();
}

bool OccupancyNode::occupied( const Vector3 & at ) const
{
	return _tree.occupied( at );
}

bool OccupancyNode::point_ajacent( const Vector3 & at ) const
{
	return _tree.point_ajacent( at );
}

bool OccupancyNode::intersects( Node * node ) const
{
	if ( !node )
		return false;
	OccupancyNode * on = Object::cast_to<OccupancyNode>( node );
	if ( !on )
		return false;
	const bool res = _tree.intersects( &(on->_tree) );
	return res;
}

bool OccupancyNode::touches( Node * node ) const
{
	if ( !node )
		return false;
	OccupancyNode * on = Object::cast_to<OccupancyNode>( node );
	if ( !on )
		return false;
	const bool res = _tree.touches( &(on->_tree) );
	return res;
}

Vector3 OccupancyNode::touch_point( Node * node ) const
{
	if ( !node )
		return Vector3();
	OccupancyNode * on = Object::cast_to<OccupancyNode>( node );
	if ( !on )
		return Vector3();
	const Vector3 res = _tree.touch_point( &(on->_tree) );
	return res;
}

bool OccupancyNode::intersects_ray( const Vector3 p_from, const Vector3 p_to ) const
{
	const bool res = _tree.intersects_ray( p_from, p_to );
	return res;
}

