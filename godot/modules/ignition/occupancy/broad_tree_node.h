
#ifndef __BROAD_TREE_NODE_H_
#define __BROAD_TREE_NODE_H_

#include "data_types.h"
#include "se3.h"
#include "data_types.h"

#include "octree_mesh.h"

#include "core/math/vector3.h"

namespace Ign
{

class BroadTree;
class OctreeMesh;
class OctreeMeshGd;

class BroadTreeNode
{
public:
    BroadTreeNode();
    virtual ~BroadTreeNode();
    BroadTreeNode( const BroadTreeNode & inst );
    const BroadTreeNode & operator=( const BroadTreeNode & inst );

    bool hasChildren() const;
    bool subdivide();

    bool inside( const OctreeMesh * om ) const;
    
    //bool objects_inside( const RigidBody * body, const CollisionObject * co, Float h, Vector<int> & collision_obj_inds ) const;

    // Initialize vertices and planes.
    void init();

	OctreeMeshGd * intersects_segment( const Vector3 & start, const Vector3 & end, OctreeMeshGd * exclude_mesh ) const;
    OctreeMeshGd * intersects_segment_face( const Vector3 & start, const Vector3 & end, real_t & ret_dist, Vector3 & at, OctreeMesh::FaceProperties & ret_face_props, OctreeMeshGd * exclude_mesh ) const;

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



