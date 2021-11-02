
#ifndef __PBD_BROAD_NODE_H_
#define __PBD_BROAD_NODE_H_

#include "data_types.h"
#include "se3.h"
#include "data_types.h"

#include "core/math/vector3.h"

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

    bool inside( OctreeMesh * om ) const;
    
    //bool objects_inside( const RigidBody * body, const CollisionObject * co, Float h, Vector<int> & collision_obj_inds ) const;

    // Initialize vertices and planes.
    void init();

    bool intersects_segment( const Vector3 & start, const Vector3 & end ) const;
    Array intersects_segment_face( const Vector3 & start, const Vector3 & end ) const;

    BroadTree * tree;
    int absIndex;
    int parentAbsIndex;
    int indexInParent;
    int level;

    // Child indices in Octtree list.
    int children[8];
    
    real_t  size2; // Size over 2.
    Vector3 center;
    AABB    aabb_;
        
    Vector<int> ptInds;
};

}


#endif



