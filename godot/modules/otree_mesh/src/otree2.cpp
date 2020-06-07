
#include "otree2.h"


static void faces_from_surface( const Mesh & mesh, int surface_idx, Vector<Face3> & faces );
static void void parse_mesh_arrays( const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces );

OTree2::OTree2( real_t node_sz )
{
    // Initialize counters and parameters.
    this->max_depth_  = -1;
    this->node_sz_    = node_sz;

    // Insert root node.
    /*
    ONode2 r;
    r.absIndex = rootIndex;
    r.level = 0;
    r.size2 = 1.0;
    r.tree  = this;
    */
}

OTree2::~OTree2()
{

}

OTree2::OTree2( const OTree2 & inst )
{
    *this = inst;
}

const OTree2 & OTree2::operator=( const OTree2 & inst )
{
    if ( this != &inst )
    {
        ptRefs     = inst.ptRefs;
        nodes      = inst.nodes;

		node_sz_   = inst.node_sz_;
        max_depth_ = inst.max_depth_;
    }
    return *this;
}

void OTree2::clear()
{
	nodes.clear();
	ptRefs.clear();
}

void OTree2::append( const Transform & t, const Ref<Mesh> mesh )
{
	const int qty = mesh->get_surface_count();
	for ( int i=0; i<qty; i++ )
	{
		//SlicerFace::faces_from_surface(**mesh, i);
		if (mesh.surface_get_primitive_type(surface_idx) != Mesh::PRIMITIVE_TRIANGLES) {
			return PoolVector<SlicerFace>();
		}
	}
}

void OTree2::subdivide()
{
}


bool OTree2::parent( const ONode2 & node, ONode2 * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int OTree2::insertNode( ONode2 & node )
{
    nodes.push_back( node );
    const int ind = static_cast<int>(nodes.size()) - 1;
	ONode2 * nns = nodes.ptrw();
	ONode2 & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;
    return ind;
}

void OTree2::updateNode( const ONode2 & node )
{
    nodes.ptrw()[ node.absIndex ] = node;
}


static void faces_from_surface( const Mesh & mesh, int surface_idx, Vector<Face3> & faces )
{
	// Don't add faces if doesn't consist of triangles.
	if (mesh.surface_get_primitive_type(surface_idx) != Mesh::PRIMITIVE_TRIANGLES)
		return;

	if ( mesh.surface_get_format(surface_idx) & Mesh::ARRAY_FORMAT_INDEX )
	{
		return parse_mesh_arrays(mesh, surface_idx, true);
	}
	else
	{
		return parse_mesh_arrays(mesh, surface_idx, false);
	}
}

static void void parse_mesh_arrays( const Mesh & mesh, int surface_idx, bool is_index_array, Vector<Face3> & faces )
{
	PoolVector<SlicerFace> faces;
	cosnt int vert_count = is_index_array ? mesh.surface_get_array_index_len( surface_idx ) :
		                                    mesh.surface_get_array_len( surface_idx );
	if (vert_count == 0 || vert_count % 3 != 0)
		return;

	const int d_qty = vert_count / 3;
	const int orig_qty = faces.size();
	faces.resize( orig_qty + d_qty );

	Array arrays = mesh.surface_get_arrays(surface_idx);
	FaceFiller filler(faces, arrays);

	if (is_index_array) {
		PoolVector<int> indices = arrays[Mesh::ARRAY_INDEX];
		auto indices_reader = indices.read();

		for (int i = 0; i < vert_count; i++) {
			filler.fill(i, indices_reader[i]);
		}
	} else {
		for (int i = 0; i < vert_count; i++) {
			filler.fill(i, i);
		}
	}

	return faces;
}


