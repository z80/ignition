
#ifndef __PBD_BROAD_TREE_H_
#define __PBD_BROAD_TREE_H_

#include "data_types.h"
#include "se3.h"
#include "pbd2_broad_node.h"
#include "pbd2_pose.h"
#include "pbd2_contact_point_bb.h"

#include "core/variant.h"

namespace Pbd
{

class CollisionObject;
class Simulation;

struct CollisionPoint
{
    Vector3d at;
    Vector3d displacement; // How to move body "A" to fix it.
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

    // These three are supposed to be used in simulation loop.
    // "subdivide" should be called once.
    void subdivide( Simulation * sim, Float h );
    // Pair of these methods is supposed to be used for each body's collision object.
    // This one determines with what objects it might intersect.
    const Vector<ContactPointBb> & find_contact_points( RigidBody * body, Float h );

    // For visualization.
    PoolVector3Array lines_nodes() const;

    // These three for tree construction.
    bool parent( const BroadTreeNode & node, BroadTreeNode * & parent );
    int  insert_node( BroadTreeNode & node );
    void update_node( const BroadTreeNode & node );

    // Selecting intersecting nodes.
    void intersecting_pairs( int ind );
    bool select_for_one( RigidBody * body, CollisionObject * co, Float h, Vector<int> & inds );
    void remove_duplicates( Vector<int> & inds );
	void remove_spatial_duplicates( Vector<ContactPointBb> & pts );

    CollisionObject * collision_object( int ind );

    Vector<CollisionObject *> collision_objects_;
    SE3                     se3_;
    Vector<BroadTreeNode>   nodes_;

    Simulation * simulation;
    //Vector<NarrowTree *>  bodies_;

    // This one is used for querying contacts in between pairs of bodies.
    Vector<ContactPointBb> contacts_;
    // This one is a helper array used in "select".
    Vector<int> collision_object_inds_;
    // 
    Vector<Vector3d> ats_;
    Vector<Vector3d> depths_;

    // Maximum subdivision level.
    int max_depth_;
    // Minimum size is determined at every iteration by taking the smallest object size.
    Float min_size_;
};


}






#endif





