
#include "broad_tree.h"
#include "ref_frame_node.h"
#include "octree_mesh.h"
#include "octree_mesh_gd.h"


namespace Ign
{

static const Float EPS = 0.0001;
static const Float MIN_CONTACT_DIST = 0.001;

static void find_mesh_instances( RefFrameNode * node, Vector<OctreeMeshGd *> & instances );


BroadTree::BroadTree()
{
    ref_frame_physics_ = nullptr;
    max_depth_         = 2;
}

BroadTree::~BroadTree()
{
}

void BroadTree::set_max_depth( int new_depth )
{
    max_depth_ = new_depth;
}

int BroadTree::get_max_depth() const
{
    return max_depth_;
}

void BroadTree::clear()
{
   octree_meshes_.clear();
   nodes_.clear();
}

void BroadTree::subdivide( RefFrameNode * ref_frame_physics )
{
    ref_frame_physics_ = ref_frame_physics;
    // Before eerything else clear nodes.
    nodes_.clear();
    octree_meshes_.clear();

    // Search for all occurences of OctreeMeshGd instances.
    find_mesh_instances( ref_frame_physics, octree_meshes_ );
    
    const int meshes_qty = octree_meshes_.size();
    if ( meshes_qty < 1 )
        return;

    bool initialized = false;
    Float x_min, x_max, y_min, y_max, z_min, z_max;
    // Initialize with the very first point.
    for ( int i=0; i<meshes_qty; i++ )
    {
        OctreeMeshGd * mesh_gd = octree_meshes_.ptr()[i];
		OctreeMesh * mesh = mesh_gd->octree_mesh();
        {
            const Vector3d c = mesh->get_origin();
            const Float sz = mesh->size2();
            x_min = x_max = c.x_;
            y_min = y_max = c.y_;
            z_min = z_max = c.z_;
            initialized = true;
            break;
        }
        if ( initialized )
            break;
    }
    if ( !initialized )
        return;

    BroadTreeNode root;

    for ( int i=0; i<meshes_qty; i++ )
    {
		OctreeMeshGd * mesh_gd = octree_meshes_.ptr()[i];
		OctreeMesh * mesh = mesh_gd->octree_mesh();
		// Determine dimensions.
        const Vector3d c  = mesh->get_origin();
        const Float    sz = mesh->size2();
        const Vector3d v_max = c + Vector3d( sz, sz, sz );
        const Vector3d v_min = c - Vector3d( sz, sz, sz );
        if ( v_min.x_ < x_min )
            x_min = v_min.x_;
        if ( v_max.x_ > x_max )
            x_max = v_max.x_;
        if ( v_min.y_ < y_min )
            y_min = v_min.y_;
        if ( v_max.y_ > y_max )
            y_max = v_max.y_;
        if ( v_min.z_ < z_min )
            z_min = v_min.z_;
        if ( v_max.z_ > z_max )
            z_max = v_max.z_;
    }

    const Vector3 c( (x_min+x_max)/2.0, (y_min+y_max)/2.0, (z_min+z_max)/2.0 );
    const Vector3 dims( (x_max-x_min)/2.0, (y_max-y_min)/2.0, (z_max-z_min)/2.0 );
    real_t d = ( dims.x > dims.y ) ? dims.x : dims.y;
    d = (d > dims.z) ? d : dims.z;

    // Just to have some gap ???
    // Not sure if it is needed.
    d *= 1.1;

    root.level  = 0;
    root.tree   = this;
    root.center = c;
    root.size2  = d;
    root.init();
	root.ptInds.clear();
	for ( int i=0; i<meshes_qty; i++ )
		root.ptInds.push_back( i );

    insert_node( root );

    root.subdivide();
    update_node( root );
}






/*PoolVector3Array BroadTree::lines_nodes( RefFrameNode * camera ) const
{
    const SE3        se3 = this->relative_to( camera );
    const Quat    q_root = se3.q();
    const Vector3 r_root = se3.r();

    Vector<Vector3> ls;
    const int qty = nodes_.size();
    for ( int i=0; i<qty; i++ )
    {
        const BroadTreeNode & n = nodes_.ptr()[i];
        const bool has_ch = n.hasChildren();
        if ( has_ch )
            continue;

        Vector3 v[8];
        const Vector3 c = n.center;
        const real_t sz = n.size2;
        v[0] = c + Vector3( -sz, -sz, -sz );
        v[1] = c + Vector3(  sz, -sz, -sz );
        v[2] = c + Vector3(  sz,  sz, -sz );
        v[3] = c + Vector3( -sz,  sz, -sz );
        v[4] = c + Vector3( -sz, -sz,  sz );
        v[5] = c + Vector3(  sz, -sz,  sz );
        v[6] = c + Vector3(  sz,  sz,  sz );
        v[7] = c + Vector3( -sz,  sz,  sz );
        
        // Convert to camera ref. frame.
        for ( int j=0; j<8; j++ )
        {
            const Vector3 r = v[j];
            v[j] = q_root.xform( r ) + r_root;
        }

        ls.push_back( v[0] );
        ls.push_back( v[1] );

        ls.push_back( v[1] );
        ls.push_back( v[2] );

        ls.push_back( v[2] );
        ls.push_back( v[3] );

        ls.push_back( v[3] );
        ls.push_back( v[0] );


        ls.push_back( v[4] );
        ls.push_back( v[5] );

        ls.push_back( v[5] );
        ls.push_back( v[6] );

        ls.push_back( v[6] );
        ls.push_back( v[7] );

        ls.push_back( v[7] );
        ls.push_back( v[4] );


        ls.push_back( v[0] );
        ls.push_back( v[4] );

        ls.push_back( v[1] );
        ls.push_back( v[5] );

        ls.push_back( v[2] );
        ls.push_back( v[6] );

        ls.push_back( v[3] );
        ls.push_back( v[7] );
    }

    PoolVector3Array res;
    const int sz = ls.size();
    res.resize( sz );
    for ( int i=0; i<sz; i++ )
    {
        const Vector3 & v = ls.ptr()[i];
        res.set( i, v );
    }

    return res;
}*/


bool BroadTree::parent( const BroadTreeNode & node, BroadTreeNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = nullptr;
        return false;
    }

    parent = &( nodes_.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int  BroadTree::insert_node( BroadTreeNode & node )
{
    nodes_.push_back( node );
    const int ind = static_cast<int>(nodes_.size()) - 1;
    BroadTreeNode * nns = nodes_.ptrw();
    BroadTreeNode & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;

    node = n;
    return ind;
}

void BroadTree::update_node( const BroadTreeNode & node )
{
    nodes_.ptrw()[ node.absIndex ] = node;
}



int BroadTree::get_octree_meshes_qty() const
{
    const int ret = octree_meshes_.size();
    return ret;
}

OctreeMeshGd * BroadTree::get_octree_mesh( int ind )
{
    const int qty = octree_meshes_.size();
    if ( ( ind < 0 ) || ( ind >= qty ) )
        return nullptr;

    OctreeMeshGd * mesh_gd = octree_meshes_.ptrw()[ind];
    return mesh_gd;
}

OctreeMeshGd * BroadTree::intersects_segment( const Vector3 & start, const Vector3 & end, OctreeMeshGd * exclude_mesh ) const
{
	if ( nodes_.empty() )
		return nullptr;
    const BroadTreeNode & root = nodes_.ptr()[0];
	OctreeMeshGd * mesh_gd = root.intersects_segment( start, end, exclude_mesh );
    return mesh_gd;
}

OctreeMeshGd * BroadTree::intersects_segment_face( const Vector3 & start, const Vector3 & end, real_t & dist, Vector3 & at, OctreeMesh::FaceProperties & face_props, OctreeMeshGd * exclude_mesh ) const
{
	if ( nodes_.empty() )
		return nullptr;
	const BroadTreeNode & root = nodes_.ptr()[0];
	dist = -1.0;
	OctreeMeshGd * mesh_gd = root.intersects_segment_face( start, end, dist, at, face_props, exclude_mesh );
    return mesh_gd;
}










static void find_mesh_instances( RefFrameNode * node, Vector<OctreeMeshGd *> & instances )
{
    const int children_qty = node->get_child_count();
    for ( int i=0; i<children_qty; i++ )
    {
        Node * ch = node->get_child( i );
        RefFrameNode * ref_frame = Node::cast_to<RefFrameNode>( ch );
        if (ref_frame == nullptr)
            continue;
        const int qty = ch->get_child_count();
        for ( int j=0; j<qty; j++ )
        {
            Node * cch = ch->get_child( j );
            OctreeMeshGd * mesh_gd = Node::cast_to<OctreeMeshGd>( cch );
            if ( mesh_gd != nullptr )
            {
                const Vector3 at = ref_frame->r();
                const Quat    q  = ref_frame->q();
				mesh_gd->set_origin( at );
				mesh_gd->set_quat( q );

                instances.push_back( mesh_gd );
                break;
            }
        }
    }
}




}
















