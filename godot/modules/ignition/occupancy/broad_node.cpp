
#include "pbd2_broad_node.h"
#include "pbd2_broad_tree.h"
#include "pbd2_simulation.h"
#include "pbd2_rigid_body.h"
#include "pbd2_collision_object.h"
#include "matrix3d.h"

namespace Pbd
{

static const Float EPS = 0.0001;

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
        tree = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent = inst.indexInParent;
        level = inst.level;
        absIndex = inst.absIndex;

        children[0] = inst.children[0];
        children[1] = inst.children[1];
        children[2] = inst.children[2];
        children[3] = inst.children[3];
        children[4] = inst.children[4];
        children[5] = inst.children[5];
        children[6] = inst.children[6];
        children[7] = inst.children[7];

        size2 = inst.size2;
        center = inst.center;
        
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

bool BroadTreeNode::subdivide( Float h )
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
            const CollisionObject * co = tree->collision_object(ind);

            const bool inside = co->inside( &ch_n, h );
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
}




















}



