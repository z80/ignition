
#ifndef __PBD_BROAD_NODE_H_
#define __PBD_BROAD_NODE_H_

#include "data_types.h"
#include "se3.h"
#include "occupancy_types.h"


namespace Ign
{

class BroadTree;
class OctreeMesh;

class BroadTreeNode
{
public:
    BroadTreeNode();
    ~BroadTreeNode();
    BroadTreeNode( const BroadTreeNode & inst );
    const BroadTreeNode & operator=( const BroadTreeNode & inst );

    bool hasChildren() const;
    bool subdivide();

    //bool objects_inside( const RigidBody * body, const CollisionObject * co, Float h, Vector<int> & collision_obj_inds ) const;

    // Initialize vertices and planes.
    void init();

    BroadTree * tree;
    int absIndex;
    int parentAbsIndex;
    int indexInParent;
    int level;

    // Child indices in Octtree list.
    int children[8];
    
    real_t   size2; // Size over 2.
    Vector3 center;
        
    Vector<int> ptInds;
};

}


#endif



