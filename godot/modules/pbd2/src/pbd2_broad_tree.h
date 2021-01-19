
#ifndef __PBD_BROAD_TREE_H_
#define __PBD_BROAD_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "pbd2_broad_node.h"
#include "pbd2_pose.h"

#include "core/variant.h"

namespace Pbd
{

class NarrowTree;

struct CollisionPoint
{
    int ind_a;
    int ind_b;
    Vector3d at;
    Vector3d displacement; // How to move body "A" to fix it.
};

struct CollidingPair
{
    int ind_a;
    int ind_b;
};

class BroadTree
{
public:
    BroadTree();
    ~BroadTree();

    // Manipulating filling up the occupancy grid.
    void set_max_depth( int new_level=5 );
    int max_depth() const;

    void clear();
    void subdivide();

    bool intersect_with_all( int ind );

    // For visualization.
    PoolVector3Array lines_nodes() const;

    // These three for tree construction.
    bool parent( const BroadTreeNode & node, BroadTreeNode * & parent );
    int  insert_node( BroadTreeNode & node );
    void update_node( const BroadTreeNode & node );

    // Selecting intersecting nodes.
    void intersecting_pairs( int ind );
    bool select_for_one( int tree_ind, Vector<int> & inds );
    void remove_duplicates();

    void collide_pairs();
    void collide_pair( int ind_a, int ind_b );

    SE3                   se3_;
    Vector<BroadTreeNode> nodes_;
    Vector<NarrowTree *>  bodies_;

    Vector<CollisionPoint> collisions_;
    // This one is a helper array used in "select".
    Vector<int> body_inds_;
    // 
    Vector<Vector3d> ats_;
    Vector<Vector3d> depths_;

    // Maximum subdivision level.
    int max_depth_;
};


}






#endif





