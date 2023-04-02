
#include "broad_tree_node.h"
#include "broad_tree.h"
#include "octree_mesh.h"
#include "octree_mesh_gd.h"
//#include "matrix3d.h"

namespace Ign
{

static const real_t EPS = 0.0001;

BroadTreeNode::BroadTreeNode()
{
    this->tree = nullptr;
    parentAbsIndex = -1;
    indexInParent = -1;
    level = -1;
    absIndex = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    size2 = 1.0;
    center = Vector3( 0.0, 0.0, 0.0 );

    init();
}

BroadTreeNode::~BroadTreeNode()
{

}

BroadTreeNode::BroadTreeNode( const BroadTreeNode & inst )
{
    *this = inst;
}

const BroadTreeNode & BroadTreeNode::operator=( const BroadTreeNode & inst )
{
    if ( this != &inst )
    {
        tree           = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent  = inst.indexInParent;
        level          = inst.level;
        absIndex       = inst.absIndex;

        children[0] = inst.children[0];
        children[1] = inst.children[1];
        children[2] = inst.children[2];
        children[3] = inst.children[3];
        children[4] = inst.children[4];
        children[5] = inst.children[5];
        children[6] = inst.children[6];
        children[7] = inst.children[7];

        size2  = inst.size2;
        center = inst.center;
        aabb_  = inst.aabb_;
        
        ptInds = inst.ptInds;

        init();
    }

    return *this;
}




bool BroadTreeNode::hasChildren() const
{
    // Filled can't have children.
    if ( !ptInds.is_empty() )
        return false;

    for ( int i=0; i<8; i++ )
    {
        if ( children[i] >= 0 )
            return true;
    }

    return false;
}

bool BroadTreeNode::subdivide()
{
    if (level >= tree->max_depth_)
    {
        return false;
    }
    
    // It can't have children because subdivision is happening.
    // But just in case.
    bool ch = hasChildren();
    if ( ch )
        return false;
    
    // If it is empty, no need to subdivide.
    // Or if there is just one object inside, also don't subdivide.
    if ( ptInds.size() < 2 )
        return false;

    const int childLevel = this->level + 1;
    const Float chSize2  = this->size2 * 0.5;

    BroadTreeNode nn[8];
    int    qtys[8];
    BroadTreeNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insert_node( n );
        nn[i] = tree->nodes_[ n.absIndex ];

        nn[i].indexInParent = i;
        nn[i].parentAbsIndex = this->absIndex;
        nn[i].level = childLevel;
        nn[i].size2 = chSize2;

        children[i] = nn[i].absIndex;

        qtys[i] = 0;
    }
    nn[0].center = this->center;
    nn[0].center.x -= chSize2;
    nn[0].center.y -= chSize2;
    nn[0].center.z -= chSize2;

    nn[1].center = this->center;
    nn[1].center.x += chSize2;
    nn[1].center.y -= chSize2;
    nn[1].center.z -= chSize2;

    nn[2].center = this->center;
    nn[2].center.x += chSize2;
    nn[2].center.y += chSize2;
    nn[2].center.z -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x -= chSize2;
    nn[3].center.y += chSize2;
    nn[3].center.z -= chSize2;

    nn[4].center = this->center;
    nn[4].center.x -= chSize2;
    nn[4].center.y -= chSize2;
    nn[4].center.z += chSize2;

    nn[5].center = this->center;
    nn[5].center.x += chSize2;
    nn[5].center.y -= chSize2;
    nn[5].center.z += chSize2;

    nn[6].center = this->center;
    nn[6].center.x += chSize2;
    nn[6].center.y += chSize2;
    nn[6].center.z += chSize2;

    nn[7].center = this->center;
    nn[7].center.x -= chSize2;
    nn[7].center.y += chSize2;
    nn[7].center.z += chSize2;

    for ( int i=0; i<8; i++ )
        nn[i].init();

    // Need to assign center and size
    const int qty = ptInds.size();
    for ( int j=0; j<8; j++ )
    {
        BroadTreeNode & ch_n = nn[j];
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds[i];
            OctreeMeshGd * mesh_gd = tree->get_octree_mesh(ind);
			OctreeMesh * mesh      = mesh_gd->octree_mesh();

            const bool inside = ch_n.inside( mesh );
            if ( inside )
            {
                ch_n.ptInds.push_back( ind );
                qtys[j] += 1;
            }
        }
    }

    // Reset indices and value for a node with children.
    ptInds.clear();

    // Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        BroadTreeNode & ch_n = nn[i];
        ch_n.subdivide();
        tree->update_node( ch_n );
    }

    return true;
}

bool BroadTreeNode::inside( const OctreeMesh * om ) const
{
    const Vector3 dr = om->get_origin() - center;
    const real_t dist = dr.length();
    const real_t max_dist = size2 + om->size2();
    const bool in = (dist <= max_dist);
    return in;
}



void BroadTreeNode::init()
{
    // AABB
    const Vector3 sz2 = Vector3( size2, size2, size2 );
    const Vector3 sz  = sz2 * 2.0;
    const Vector3 origin = center - sz2;
    aabb_ = AABB( origin, sz );
}


OctreeMeshGd * BroadTreeNode::intersects_segment( const Vector3 & start, const Vector3 & end, OctreeMeshGd * exclude_mesh ) const
{
    const bool intersects_aabb = aabb_.intersects_ray( start, end );
    if ( !intersects_aabb )
        return nullptr;

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int mesh_ind = ptInds.ptr()[i];
            OctreeMeshGd * mesh_gd = tree->octree_meshes_.ptr()[mesh_ind];
			if ( mesh_gd == exclude_mesh )
				continue;
			OctreeMesh   * mesh    = mesh_gd->octree_mesh();
            const bool ok = mesh->intersects_segment( start, end );
            if ( ok )
                return mesh_gd;
        }
        return nullptr;
    }

    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const BroadTreeNode & ch_n = tree->nodes_.ptr()[ind];
        OctreeMeshGd * mesh_gd = ch_n.intersects_segment( start, end, exclude_mesh );
        if ( mesh_gd != nullptr )
            return mesh_gd;
    }

    return nullptr;
}

OctreeMeshGd *  BroadTreeNode::intersects_segment_face( const Vector3 & start, const Vector3 & end, real_t & ret_dist, Vector3 & ret_at, OctreeMesh::FaceProperties & ret_face_props, OctreeMeshGd * exclude_mesh ) const
{
    const bool intersects_aabb = aabb_.intersects_segment( start, end );
    if ( !intersects_aabb )
    {
        return nullptr;
    }

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        bool intersects = false;
        real_t                     best_face_dist = -1.0;
		Vector3                    best_face_at;
        OctreeMesh::FaceProperties best_face_properties;
		OctreeMeshGd             * best_octree_mesh = nullptr;
        for ( int i=0; i<qty; i++ )
        {
            const int child_face_ind = ptInds.ptr()[i];
            OctreeMeshGd * mesh_gd = tree->octree_meshes_[child_face_ind];
			if ( mesh_gd == exclude_mesh )
				continue;
			const OctreeMesh   * mesh    = mesh_gd->octree_mesh();

            real_t             face_dist;
			Vector3            face_at;
            OctreeMesh::FaceProperties face_props;
            const bool ok = mesh->intersects_segment_face( start, end, face_dist, face_at, face_props );
            if ( ok )
            {
                if ( (!intersects) || (face_dist < best_face_dist) )
                {
                    best_face_dist       = face_dist;
					best_face_at         = face_at;
                    best_face_properties = face_props;
					best_octree_mesh     = mesh_gd;
                }
                intersects = true;
                // Do not interrupt here.
                // Need to check all triangles.
            }
        }
        if ( intersects )
        {
            ret_dist       = best_face_dist;
			ret_at         = best_face_at;
            ret_face_props = best_face_properties;
        }
        return best_octree_mesh;
    }

    real_t                     best_face_dist = -1.0;
	Vector3                    best_face_at;
    OctreeMesh::FaceProperties best_face_properties;
	OctreeMeshGd             * best_octree_mesh = nullptr;
    bool           intersects = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const BroadTreeNode & ch_n = tree->nodes_.ptr()[ind];
        real_t                     face_dist;
		Vector3                    face_at;
        OctreeMesh::FaceProperties face_props;
        OctreeMeshGd * mesh_gd = ch_n.intersects_segment_face( start, end, face_dist, face_at, face_props, exclude_mesh );
        if ( mesh_gd )
        {
            if ( (!intersects) || (face_dist < best_face_dist) )
            {
                best_face_dist       = face_dist;
				best_face_at         = face_at;
                best_face_properties = face_props;
				best_octree_mesh     = mesh_gd;
            }
            intersects = true;
        }
        // Again, don't interrupt it here. Need to check all the children.
    }

    if ( intersects )
    {
        ret_dist       = best_face_dist;
		ret_at         = best_face_at;
        ret_face_props = best_face_properties;
    }
    return best_octree_mesh;
}


















}



