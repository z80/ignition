
#include "occupancy_tree.h"

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face3> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces );


OccupancyTree::OccupancyTree()
{
	// Initialize counters and parameters.
	this->max_depth_  = -1;
	this->node_sz_    = 0.1;
}

OccupancyTree::~OccupancyTree()
{
}

void OccupancyTree::setNodeSize( real_t sz )
{
	node_sz_ = sz;
}

real_t OccupancyTree::nodeSize() const
{
	return node_sz_;
}

void OccupancyTree::clear()
{
	nodes_.clear();
	faces_.clear();
}

void OccupancyTree::append( const Transform & t, const Ref<Mesh> mesh )
{
	const int qty = mesh->get_surface_count();
	for ( int i=0; i<qty; i++ )
	{
		const Mesh & m = **mesh;
		faces_from_surface( t, m, i, faces_ );
	}
}

void OccupancyTree::subdivide()
{
	// Get average of all the face points.
	const int qty = faces_.size();
	Vector3 c;
	c.zero();
	for ( int i=0; i<qty; i++ )
	{
		const Face3 & f = faces_.ptr()[i];
		c += f.vertex[0];
		c += f.vertex[1];
		c += f.vertex[2];
	}
	const real_t s = 1.0 / static_cast<real_t>( qty );
	c *= s;

	// Here adjust it to be multiple of grid size.
	const real_t sz = node_sz_ * 2.0;
	c.x = sz * Math::round( c.x / sz );
	c.y = sz * Math::round( c.y / sz );
	c.z = sz * Math::round( c.z / sz );

	// Here find the largest distance.
	real_t d = 0.0;
	for ( int i=0; i<qty; i++ )
	{
		const Face3 & f = faces_.ptr()[i];
		for ( int j=0; j<3; j++ )
		{
			const Vector3 dv = f.vertex[j] - c;
			real_t l = dv.length();
			if ( l > d )
				d = l;
		}
	}

	real_t level = Math::log( d / node_sz_) / Math::log( 2.0 );
	level = Math::ceil( level );
	const int level_int = static_cast<int>( level );
	d = node_sz_ * Math::pow( 2.0, level_int );
	this->max_depth_ = level_int;

	nodes_.clear();
	OccupancyTreeNode root;
	root.level = 0;
	root.tree = this;
	root.center = c;
	root.size2  = d;
	root.value  = qty;
	root.ptInds.clear();
	for ( int i=0; i<qty; i++ )
		root.ptInds.push_back( i );
	root.init();
	insert_node( root );

	// Debugging. All the faces should be inside the root node.
	/*{
	int inside_qty = 0;
	for ( int i=0; i<qty; i++ )
	{
	const Face3 & f = faces_.ptr()[i];
	const bool inside = root.inside( f );
	if ( inside )
	inside_qty += 1;
	else
	{
	bool inside2 = root.inside( f );
	inside2 = inside2 || false;
	}
	}
	inside_qty += 0;
	}*/


	root.subdivide();
	nodes_.ptrw()[ 0 ] = root;
}

bool OccupancyTree::occupied( const Vector3 & at ) const
{
	const OccupancyTreeNode & root = nodes_.ptr()[0];
	const bool pt_inside = point_inside( root, at );
	return pt_inside;
}

bool OccupancyTree::point_inside( const OccupancyTreeNode & n, const Vector3 & at ) const
{
	const bool pt_in = n.inside( at );
	if ( !pt_in )
		return false;
	const bool has_ch = n.hasChildren();
	if ( !has_ch )
	{
		// Check if if occupied.
		const bool res = ( n.value > 0 );
		return res;
	}

	// Check children recursively.
	for ( int i=0; i<8; i++ )
	{
		const int ch_ind = n.children[i];
		if ( ch_ind < 0 )
			return false;
		const OccupancyTreeNode & ch_n = nodes_.ptr()[ch_ind];
		const bool ch_inside = point_inside( ch_n, at );
		if ( ch_inside )
			return true;
	}

	return false;
}

bool OccupancyTree::point_ajacent( const Vector3 & at ) const
{
	Vector3 c( at );
	const real_t sz = node_sz_ * 2.0;
	c.x = Math::round( c.x / sz ) * sz;
	c.y = Math::round( c.y / sz ) * sz;
	c.z = Math::round( c.z / sz ) * sz;
	// Test 6 possibilities.
	{
		const Vector3 v( c.x, c.y, c.z+sz );
		const bool inside = occupied( v );
		if ( inside )
			return true;
	}
	{
		const Vector3 v( c.x, c.y+sz, c.z );
		const bool inside = occupied( v );
		if ( inside )
			return true;
	}
	{
		const Vector3 v( c.x+sz, c.y, c.z );
		const bool inside = occupied( v );
		if ( inside )
			return true;
	}
	{
		const Vector3 v( c.x, c.y, c.z-sz );
		const bool inside = occupied( v );
		if ( inside )
			return true;
	}
	{
		const Vector3 v( c.x, c.y-sz, c.z );
		const bool inside = occupied( v );
		if ( inside )
			return true;
	}
	{
		const Vector3 v( c.x-sz, c.y, c.z );
		const bool inside = occupied( v );
		if ( inside )
			return true;
	}
	return false;
}

bool OccupancyTree::intersects( const OccupancyTree * tree ) const
{
	if ( !tree )
		return false;

	const OccupancyTreeNode & root = nodes_.ptr()[0];

	const bool is_intersecting = node_intersects( root, *tree );

	return is_intersecting;
}

bool OccupancyTree::node_intersects( const OccupancyTreeNode & n, const OccupancyTree & tree ) const
{
	const OccupancyTreeNode & other_root = tree.nodes_.ptr()[0];
	const bool is_intersecting = n.inside( other_root );
	if ( !is_intersecting )
		return false;
	const bool has_ch = n.hasChildren();
	if ( !has_ch )
	{
		const bool is_filled = (n.value > 0);
		return is_filled;
	}

	for ( int i=0; i<8; i++ )
	{
		const int ch_ind = n.children[i];
		const OccupancyTreeNode & ch_n = n.tree->nodes_.ptr()[ch_ind];
		const bool ch_intersects = node_intersects( ch_n, tree );
		if ( ch_intersects )
			return true;
	}
	return false;
}

bool OccupancyTree::touches( const OccupancyTree * tree ) const
{
	if ( !tree )
		return false;
	const OccupancyTree & g = *tree;

	const int qty = g.nodes_.size();
	for ( int i=0; i<qty; i++ )
	{
		const OccupancyTreeNode & n = g.nodes_.ptr()[i];
		const bool has_ch = n.hasChildren();
		if ( has_ch )
			continue;
		const bool ch_filled = (n.value > 0);
		if ( !ch_filled )
			continue;
		const Vector3 c = n.center;
		const bool is_touching = point_ajacent( c );
		if ( is_touching )
			return true;
	}

	return false;
}

Vector3 OccupancyTree::touch_point( const OccupancyTree * tree ) const
{
	if ( !tree )
		return Vector3( 0.0, 0.0, 0.0 );
	const OccupancyTree & g = *tree;

	Vector3 tp( 0.0, 0.0, 0.0 );
	const real_t sz = node_sz_ * 2.0;

	const int qty = g.nodes_.size();
	int touch_qty = 0;
	for ( int i=0; i<qty; i++ )
	{
		const OccupancyTreeNode & n = g.nodes_.ptr()[i];
		const bool has_ch = n.hasChildren();
		if ( has_ch )
			continue;
		const bool ch_filled = (n.value > 0);
		if ( !ch_filled )
			continue;
		const Vector3 c = n.center;
		// Check 6 possibilities.
		// Test 6 possibilities.
		{
			const Vector3 v( c.x, c.y, c.z+sz );
			const bool inside = occupied( v );
			if ( inside )
			{
				touch_qty += 1;
				tp += Vector3( c.x, c.y, c.z+node_sz_ );
			}
		}
		{
			const Vector3 v( c.x, c.y+sz, c.z );
			const bool inside = occupied( v );
			if ( inside )
			{
				touch_qty += 1;
				tp += Vector3( c.x, c.y+node_sz_, c.z );
			}
		}
		{
			const Vector3 v( c.x+sz, c.y, c.z );
			const bool inside = occupied( v );
			if ( inside )
			{
				touch_qty += 1;
				tp += Vector3( c.x+node_sz_, c.y, c.z );
			}
		}
		{
			const Vector3 v( c.x, c.y, c.z-sz );
			const bool inside = occupied( v );
			if ( inside )
			{
				touch_qty += 1;
				tp += Vector3( c.x, c.y, c.z-node_sz_ );
			}
		}
		{
			const Vector3 v( c.x, c.y-sz, c.z );
			const bool inside = occupied( v );
			if ( inside )
			{
				touch_qty += 1;
				tp += Vector3( c.x, c.y-node_sz_, c.z );
			}
		}
		{
			const Vector3 v( c.x-sz, c.y, c.z );
			const bool inside = occupied( v );
			if ( inside )
			{
				touch_qty += 1;
				tp += Vector3( c.x-node_sz_, c.y, c.z );
			}
		}
	}

	if ( touch_qty > 0 )
	{
		tp = tp * ( 1.0 / static_cast<real_t>( touch_qty ) );
	}
	return tp;
}

bool OccupancyTree::intersects_ray( const Vector3 p_from, const Vector3 p_to ) const
{
	const OccupancyTreeNode & root = nodes_.ptr()[0];
	const bool res = root.intersects_ray( p_from, p_to );
	return res;
}

void OccupancyTree::set_position( const Vector3 & at )
{
	OccupancyTreeNode & root = nodes_.ptrw()[0];
	set_node_position( root, root.center, at );
}

Vector3 OccupancyTree::get_position() const
{
	const OccupancyTreeNode & root = nodes_.ptr()[0];
	return root.center;
}

void OccupancyTree::set_node_position( OccupancyTreeNode & n, const Vector3 from, const Vector3 to )
{
	n.center = n.center - from + to;
	const real_t sz = node_sz_;
	n.center.x = Math::round( n.center.x / sz ) * sz;
	n.center.y = Math::round( n.center.y / sz ) * sz;
	n.center.z = Math::round( n.center.z / sz ) * sz;
	n.init();

	const bool has_ch = n.hasChildren();
	if ( !has_ch )
		return;

	for ( int i=0; i<8; i++ )
	{
		const int ch_ind = n.children[i];
		if ( ch_ind < 0 )
			continue;
		OccupancyTreeNode & ch_n = nodes_.ptrw()[ch_ind];
		set_node_position( ch_n, from, to );
	}
}

PoolVector<Vector3> OccupancyTree::lines()
{
	Vector<Vector3> ls;
	const int qty = nodes_.size();
	for ( int i=0; i<qty; i++ )
	{
		const OccupancyTreeNode & n = nodes_.ptr()[i];
		const bool has_ch = n.hasChildren();
		if ( has_ch )
			continue;
		const bool occupied = (n.value > 0);
		if ( !occupied )
			continue;

		const Vector3 * vs = n.verts_;
		ls.push_back( vs[0] );
		ls.push_back( vs[1] );

		ls.push_back( vs[1] );
		ls.push_back( vs[2] );

		ls.push_back( vs[2] );
		ls.push_back( vs[3] );

		ls.push_back( vs[3] );
		ls.push_back( vs[0] );


		ls.push_back( vs[4] );
		ls.push_back( vs[5] );

		ls.push_back( vs[5] );
		ls.push_back( vs[6] );

		ls.push_back( vs[6] );
		ls.push_back( vs[7] );

		ls.push_back( vs[7] );
		ls.push_back( vs[4] );


		ls.push_back( vs[0] );
		ls.push_back( vs[4] );

		ls.push_back( vs[1] );
		ls.push_back( vs[5] );

		ls.push_back( vs[2] );
		ls.push_back( vs[6] );

		ls.push_back( vs[3] );
		ls.push_back( vs[7] );
	}

	PoolVector<Vector3> res;
	const int sz = ls.size();
	res.resize( sz );
	PoolVector<Vector3>::Write w = res.write();
	for ( int i=0; i<sz; i++ )
	{
		const Vector3 & v = ls.ptr()[i];
		w[i] = v;
	}

	return res;
}


bool OccupancyTree::parent( const OccupancyTreeNode & node, OccupancyTreeNode * & parent )
{
	if ( node.parentAbsIndex < 0 )
	{
		parent = 0;
		return false;
	}

	parent = &( nodes_.ptrw()[ node.parentAbsIndex ] );
	return true;
}

int  OccupancyTree::insert_node( OccupancyTreeNode & node )
{
	nodes_.push_back( node );
	const int ind = static_cast<int>(nodes_.size()) - 1;
	OccupancyTreeNode * nns = nodes_.ptrw();
	OccupancyTreeNode & n = nns[ind];
	n.tree     = this;
	n.absIndex = ind;
	return ind;
}

void OccupancyTree::update_node( const OccupancyTreeNode & node )
{
	nodes_.ptrw()[ node.absIndex ] = node;
}















static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face3> & faces )
{
	// Don't add faces if doesn't consist of triangles.
	if (mesh.surface_get_primitive_type(surface_idx) != Mesh::PRIMITIVE_TRIANGLES)
		return;

	if ( mesh.surface_get_format(surface_idx) & Mesh::ARRAY_FORMAT_INDEX )
	{
		return parse_mesh_arrays( t, mesh, surface_idx, true, faces );
	}
	else
	{
		return parse_mesh_arrays( t, mesh, surface_idx, false, faces );
	}
}

static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces )
{
	const int vert_count = is_index_array ? mesh.surface_get_array_index_len( surface_idx ) :
		mesh.surface_get_array_len( surface_idx );
	if (vert_count == 0 || vert_count % 3 != 0)
		return;

	const int d_qty = vert_count / 3;
	const int orig_qty = faces.size();
	faces.resize( orig_qty + d_qty );

	Array arrays = mesh.surface_get_arrays( surface_idx );
	//FaceFiller filler(faces, arrays);

	PoolVector<int> indices = arrays[Mesh::ARRAY_INDEX];
	PoolVector<int>::Read indices_reader = indices.read();

	PoolVector<Vector3> vertices = arrays[Mesh::ARRAY_VERTEX];
	PoolVector<Vector3>::Read vertices_reader = vertices.read();

	if ( is_index_array )
	{
		for (int i = 0; i < vert_count; i++)
		{
			const int face_idx   = i / 3;
			const int set_offset = i % 3;
			const int lookup_index = indices_reader[i];
			Face3 & face = faces.ptrw()[ orig_qty + face_idx ];
			const Vector3 v = vertices_reader[lookup_index];
			const Vector3 vt = t.xform( v );
			face.vertex[set_offset] = vt;
		}
	}
	else
	{
		for (int i = 0; i < vert_count; i++)
		{
			const int face_idx   = i / 3;
			const int set_offset = i % 3;
			const int lookup_index = i;
			Face3 & face = faces.ptrw()[ orig_qty + face_idx ];
			const Vector3 v = vertices_reader[lookup_index];
			const Vector3 vt = t.xform( v );
			face.vertex[set_offset] = vt;
		}
	}
}



