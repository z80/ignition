
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
    if ( !ptInds.empty() )
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
    if ( (level >= tree->max_depth_) || (size2 <= tree->min_size_) )
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
    nn[0].center.x_ -= chSize2;
    nn[0].center.y_ -= chSize2;
    nn[0].center.z_ -= chSize2;

    nn[1].center = this->center;
    nn[1].center.x_ += chSize2;
    nn[1].center.y_ -= chSize2;
    nn[1].center.z_ -= chSize2;

    nn[2].center = this->center;
    nn[2].center.x_ += chSize2;
    nn[2].center.y_ += chSize2;
    nn[2].center.z_ -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x_ -= chSize2;
    nn[3].center.y_ += chSize2;
    nn[3].center.z_ -= chSize2;

    nn[4].center = this->center;
    nn[4].center.x_ -= chSize2;
    nn[4].center.y_ -= chSize2;
    nn[4].center.z_ += chSize2;

    nn[5].center = this->center;
    nn[5].center.x_ += chSize2;
    nn[5].center.y_ -= chSize2;
    nn[5].center.z_ += chSize2;

    nn[6].center = this->center;
    nn[6].center.x_ += chSize2;
    nn[6].center.y_ += chSize2;
    nn[6].center.z_ += chSize2;

    nn[7].center = this->center;
    nn[7].center.x_ -= chSize2;
    nn[7].center.y_ += chSize2;
    nn[7].center.z_ += chSize2;

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
            const OctreeMesh * om = tree->get_octree_mesh(ind);

            const bool inside = om->inside( &ch_n, h );
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
        ch_n.subdivide( h );
        tree->update_node( ch_n );
    }

    return true;
}

bool BroadTreeNode::inside( OctreeMesh * om ) const
{
    const Vector3 dr = om->get_origin() - center;
    const real_t dist = dr.length();
    const real_t max_dist = size2 + om->size2();
    const bool in = (dist <= max_dist);
    return in;
}

bool BroadTreeNode::objects_inside( const RigidBody * body, const CollisionObject * co, Float h, Vector<int> & collision_obj_inds ) const
{
    const bool object_inside_this_node = co->inside( this, h );
    if ( !object_inside_this_node )
        return false;

    const bool has_children = hasChildren();
    if ( !has_children )
    {
        const int qty = ptInds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds.ptr()[i];
            const CollisionObject * co_candidate = tree->collision_object( ind );
            const bool already_processed = co_candidate->is_processed();
            if ( already_processed )
                continue;
            const RigidBody * body_candidate = co_candidate->rigid_body;
            if ( ( body != body_candidate ) && ( co != co_candidate ) )
            {
                // First check if it is already there.
                // It is because one and the same object may be in a few different nodes.
                const int already_ind = collision_obj_inds.find( ind );
                if ( already_ind < 0 )
                    collision_obj_inds.push_back( ind );
            }
        }
        return true;
    }

    bool ret = false;
    for ( int i=0; i<8; i++ )
    {
        const int child_ind = children[i];
        const BroadTreeNode & child = tree->nodes_.ptr()[child_ind];
        const bool ok = child.objects_inside( body, co, h, collision_obj_inds );
        ret = ret || ok;
    }
    return ret;
}



void BroadTreeNode::init()
{
    // AABB
    const Vector3 sz2 = Vector3( size2, size2, size2 );
    const Vector3 sz  = sz2 * 2.0;
    const Vector3 origin = center - sz2;
    aabb_ = AABB( origin, sz );
}


bool BroadTreeNode::intersects_segment( const Vector3 & start, const Vector3 & end ) const
{
    const bool intersects_aabb = aabb_.intersects_ray( start, end );
    if ( !intersects_aabb )
        return false;

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int face_ind = ptInds.ptr()[i];
            const Face3 & f = tree->faces_[face_ind];
            const bool ok = f.intersects_segment( start, end );
            if ( ok )
                return true;
        }
        return false;
    }

    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const BroadTreeNode & ch_n = tree->nodes_.ptr()[ind];
        const bool ch_intersects = ch_n.intersects_ray( start, end );
        if ( ch_intersects )
            return true;
    }

    return false;
}

Array BroadTreeNode::intersects_segment_face( const Vector3 & start, const Vector3 & end ) const
{
    const bool intersects_aabb = aabb_.intersects_segment( start, end );
    if ( !intersects_aabb )
        return false;

    const bool has_ch = hasChildren();
    if ( !has_ch )
    {
        const int qty = ptInds.size();
        bool intersects = false;
        for ( int i=0; i<qty; i++ )
        {
            const int child_face_ind = ptInds.ptr()[i];
            const Face3 & f = tree->faces_[child_face_ind];
            Vector3 at;
            const bool ok = f.intersects_segment( start, end, &at );
            if ( ok )
            {
                const Vector3 dr = at - start;
                const real_t d   = dr.length();
                if ( (face_ind < 0) || (d < dist) )
                {
                    face_ind = child_face_ind;
                    dist     = d;
                    intersects = true;
                    // Do not interrupt here.
                    // Need to check all triangles.
                }
            }
        }
        return intersects;
    }

    bool intersects = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = children[i];
        const BroadTreeNode & ch_n = tree->nodes_.ptr()[ind];
        const bool ch_intersects = ch_n.intersects_ray_face( start, end, face_ind, dist );
        if ( ch_intersects )
            intersects = true;
        // Again, don't interrupt it here. Need to check all the children.
    }

    return intersects;
}


















}



