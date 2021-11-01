
#ifndef __PBD_BROAD_TREE_H_
#define __PBD_BROAD_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "broad_node.h"

#include "core/variant.h"

namespace Ign
{

class RefFrameNode;
class OctreeMeshGd;

class BroadTree
{
public:
    BroadTree();
    ~BroadTree();

    // Manipulating filling up the occupancy grid.
    void set_max_depth( int new_level=5 );
    int  max_depth() const;

    void clear();

    // These three are supposed to be used in simulation loop.
    // "subdivide" should be called once.
    void subdivide( RefFrameNode * ref_frame_physics );

    // For visualization.
    PoolVector3Array lines_nodes( RefFrameNode * camera ) const;

    // These three for tree construction.
    bool parent( const BroadTreeNode & node, BroadTreeNode * & parent );
    int  insert_node( BroadTreeNode & node );
    void update_node( const BroadTreeNode & node );

    int          get_octree_meshes_qty() const;
    OctreeMesh * get_octree_mesh( int ind );

    Vector<OctreeMeshGd *>    octree_meshes_;
    SE3                       se3_;
    Vector<BroadTreeNode>     nodes_;

    RefFrameNode * ref_frame_physics_;

    // Maximum subdivision level.
    int   max_depth_;
    // Minimum size is determined at every iteration by taking the smallest object size.
    Float min_size_;
};


}






#endif





