
#ifndef __PBD_NARROW_TREE_PTS_NODE_H_
#define __PBD_NARROW_TREE_PTS_NODE_H_

#include "data_types.h"
#include "pbd2_pose.h"
#include "occupancy_types.h"


namespace Pbd
{

class NarrowTree;
class NarrowTreeSdfNode;

class NarrowTreePtsNode
{
public:
    NarrowTreePtsNode();
    ~NarrowTreePtsNode();
    NarrowTreePtsNode( const NarrowTreePtsNode & inst );
    const NarrowTreePtsNode & operator=( const NarrowTreePtsNode & inst );

    bool hasChildren() const;
    bool subdivide();

    bool point_inside( const Vector3d & at ) const;

    // Initialize vertices and planes.
    void init();

    bool collide_backward( const Pose & se3_rel, const NarrowTreeSdfNode * this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const;
    bool collide_points( const NarrowTreeSdfNode * this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const;

    // Compute "se3_optimized_" and "cube_optimized_".
    bool compute_cube_optimized();

    NarrowTree * tree;
    int absIndex;
    int parentAbsIndex;
    int indexInParent;
    int level;

    // Child indices in Octtree list.
    int children[8];

    Float    size2; // Size over 2.
    Vector3d center;
        
    Cube cube_;
    Cube cube_optimized_;

    Vector<int> ptInds;
};

}


#endif



