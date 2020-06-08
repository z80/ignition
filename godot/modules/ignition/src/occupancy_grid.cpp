
#include "otree2.h"
#include "core/math/transform.h"

static void faces_from_surface( const Transform & t, const Mesh & mesh, int surface_idx, Vector<Face3> & faces );
static void parse_mesh_arrays( const Transform & t, const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces );


void OccupancyGrid::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_node_size"), &OccupancyGrid::setNodeSize, Variant::NIL);
	ClassDB::bind_method( D_METHOD("get_node_size"), &OccupancyGrid::nodeSize, Variant::REAL);
	ClassDB::bind_method( D_METHOD("clear"), &OccupancyGrid::clear, Variant::NIL);
	ClassDB::bind_method( D_METHOD("append", "transform", "mesh"), &OccupancyGrid::append, Variant::NIL);
	ClassDB::bind_method( D_METHOD("subdivide"), &OccupancyGrid::subdivide, Variant::NIL);
}

OccupancyGrid::OccupancyGrid()
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

OccupancyGrid::OccupancyGrid( const OccupancyGrid & inst )
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
}

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


