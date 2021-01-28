
#ifndef __PBD_NARROW_TREE_H_
#define __PBD_NARROW_TREE_H_

#include "pbd2_collision_object.h"
#include "data_types.h"
#include "pbd2_narrow_tree_sdf_node.h"
#include "pbd2_narrow_tree_pts_node.h"
#include "pbd2_pose.h"

#include "scene/3d/mesh_instance.h"


namespace Pbd
{

/**
This data structure is for determining empty/filled space only.
Nonempty leaf node means space is filled. Else means it's empty.
**/

class RigidBody;

class NarrowTree: public CollisionObject
{
public:
    NarrowTree();
    ~NarrowTree();

    // Manipulating filling up the occupancy grid.
    void set_min_depth( int new_level=2 );
    int min_depth() const;
    void set_max_depth( int new_level=5 );
    int max_depth() const;
    void set_max_error( const Float new_err );
    Float max_error() const;
    void set_min_points( int new_qty );
    int min_points() const;

    Float bounding_radius() const override;
    bool inside( const BroadTreeNode * n, Float h ) const override;


    void clear();
    void append( const Transform & t, const Ref<Mesh> & mesh );
    void append_triangle( const Vector3d & a, const Vector3d & b, const Vector3d & c );
    // "subdivide" should be called.
    void subdivide();
    // These two are used by the upper one.
    // These shouldn't be used individually.
    void subdivide_sdf();
    void remove_pt_duplicates();
    void subdivide_pts();

    void intersect( CollisionObject * b, Vector<Vector3d> & ats, Vector<Vector3d> & depths ) override;
    bool intersect_sdf( NarrowTree * tree, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const;
	bool intersect_brute_force( NarrowTree * tree_b, Vector<Vector3d> & pts, Vector<Vector3d> & depths )const;

    // For visualization.
    PoolVector3Array lines_sdf_nodes() const;
    PoolVector3Array lines_surface_pts() const;
    PoolVector3Array lines_pts_nodes() const;
    PoolVector3Array lines_aligned_nodes() const;

    // These three for tree construction.
    bool parent_sdf( const NarrowTreeSdfNode & node, NarrowTreeSdfNode * & parent );
    bool parent_pts( const NarrowTreePtsNode & node, NarrowTreePtsNode * & parent );
    int  insert_node_sdf( NarrowTreeSdfNode & node );
    int  insert_node_pts( NarrowTreePtsNode & node );
    void update_node_sdf( const NarrowTreeSdfNode & node );
    void update_node_pts( const NarrowTreePtsNode & node );

    // Swapping two nodes in the array.
    void swap_nodes_sdf( int i, int j );
    void remove_node_sdf( int i );
    // Here by value as array changes while it runs.
    void merge_nodes_on_either_side( const NarrowTreeSdfNode n );

    Vector<NarrowTreeSdfNode> nodes_sdf_;
    Vector<NarrowTreePtsNode> nodes_pts_;
    Vector<Face>              faces_;
    Vector<Vector3d>          pts_;

    // Unconditionally subdivide to this level.
    int   min_depth_;
    // Subdivide SDF node if it's error is bigger than this.
    Float max_sdf_error_;
    // Unconditionally stop subdividing if level has reached this value.
    // Maximum subdivision level.
    int max_depth_;
    // Min points in a node while keeping subdividing.
    int min_pts_;

    // Should be filled based on speed and time step.
    Float speed_margin;
};


}






#endif





