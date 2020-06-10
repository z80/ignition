
#include "occupancy_grid.h"
#include "core/math/transform.h"
#include "core/math/math_funcs.h"

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face3> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces );


void OccupancyGrid::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_node_size"), &OccupancyGrid::setNodeSize, Variant::NIL);
	ClassDB::bind_method( D_METHOD("get_node_size"), &OccupancyGrid::nodeSize, Variant::REAL);
	ClassDB::bind_method( D_METHOD("clear"), &OccupancyGrid::clear, Variant::NIL);
	ClassDB::bind_method( D_METHOD("append", "transform", "mesh"), &OccupancyGrid::append, Variant::NIL);
	ClassDB::bind_method( D_METHOD("subdivide"), &OccupancyGrid::subdivide, Variant::NIL);
	ClassDB::bind_method( D_METHOD("lines"), &OccupancyGrid::subdivide, Variant::POOL_VECTOR3_ARRAY);
}

OccupancyGrid::OccupancyGrid()
	: Node()
{
    // Initialize counters and parameters.
    this->max_depth_  = -1;
    this->node_sz_    = 0.1;

    // Insert root node.
    /*
    GridNode r;
    r.absIndex = rootIndex;
    r.level = 0;
    r.size2 = 1.0;
    r.tree  = this;
    */
}

OccupancyGrid::~OccupancyGrid()
{

}

/*OccupancyGrid::OccupancyGrid( const OccupancyGrid & inst )
{
    *this = inst;
}

const OccupancyGrid & OccupancyGrid::operator=( const OccupancyGrid & inst )
{
    if ( this != &inst )
    {
		faces_     = inst.faces_;
        nodes_     = inst.nodes_;

		node_sz_   = inst.node_sz_;
        max_depth_ = inst.max_depth_;
    }
    return *this;
}*/

void OccupancyGrid::setNodeSize( real_t sz )
{
	node_sz_ = sz;
}

real_t OccupancyGrid::nodeSize() const
{
	return node_sz_;
}

void OccupancyGrid::clear()
{
	nodes_.clear();
	faces_.clear();
}

void OccupancyGrid::append( const Transform & t, const Ref<Mesh> mesh )
{
	const int qty = mesh->get_surface_count();
	for ( int i=0; i<qty; i++ )
	{
		const Mesh & m = **mesh;
		faces_from_surface( t, m, i, faces_ );
	}
}

void OccupancyGrid::subdivide()
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
	const real_t s = 1.0 / static_cast<real_t>( qty * 3 );
	c *= s;

	// Here adjust it to be multiple of grid size.

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
				l = d;
		}
	}

	real_t level = Math::log( d / node_sz_) / Math::log( 2.0 );
	level = Math::ceil( level );
	const int level_int = static_cast<int>( level );
	d = Math::pow( node_sz_, level_int );

	nodes_.clear();
	GridNode root;
	root.tree = this;
	root.center = c;
	root.size2  = d;
	root.value  = qty;
	root.ptInds.clear();
	for ( int i=0; i<qty; i++ )
		root.ptInds.push_back( i );
	root.init();
	insertNode( root );
	root.subdivide();
}

bool OccupancyGrid::occupied( const Vector3 & at )
{
	GridNode & root = nodes_.ptrw()[0];
	const bool pt_inside = pointInside( root, at );
	return pt_inside;
}

bool OccupancyGrid::pointInside( const GridNode & n, const Vector3 & at )
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
		const GridNode & ch_n = nodes_.ptr()[ch_ind];
		const bool ch_inside = pointInside( ch_n, at );
		if ( ch_inside )
			return true;
	}

	return false;
}

void OccupancyGrid::set_position( const Vector3 & at )
{
	GridNode & root = nodes_.ptrw()[0];
	set_node_position( root, root.center, at );
}

void OccupancyGrid::set_node_position( GridNode & n, const Vector3 & from, const Vector3 & to )
{
	n.center = n.center - from + to;
	const bool has_ch = n.hasChildren();
	if ( !has_ch )
		return;

	for ( int i=0; i<8; i++ )
	{
		const int ch_ind = n.children[i];
		if ( ch_ind < 0 )
			continue;
		GridNode & ch_n = nodes_.ptrw()[ch_ind];
		set_node_position( ch_n, from, to );
	}
}

PoolVector<Vector3> OccupancyGrid::lines()
{
	Vector<Vector3> ls;
	const int qty = nodes_.size();
	for ( int i=0; i<qty; i++ )
	{
		const GridNode & n = nodes_.ptr()[i];
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
	for ( int i=0; i<qty; i++ )
		res[i] = ls.ptr()[i];

	return res;
}

bool OccupancyGrid::parent( const GridNode & node, GridNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes_.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int OccupancyGrid::insertNode( GridNode & node )
{
    nodes_.push_back( node );
    const int ind = static_cast<int>(nodes_.size()) - 1;
	GridNode * nns = nodes_.ptrw();
	GridNode & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;
    return ind;
}

void OccupancyGrid::updateNode( const GridNode & node )
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
		PoolVector<int> indices = arrays[Mesh::ARRAY_INDEX];
		auto indices_reader = indices.read();

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


