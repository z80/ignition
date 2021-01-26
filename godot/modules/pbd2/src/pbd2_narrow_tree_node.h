
#ifndef __PBD2_NARROW_TREE_NODE_H_
#define __PBD2_NARROW_TREE_NODE_H_

#include "pbd2_collision_object_node.h"
#include "pbd2_narrow_tree.h"

namespace Pbd
{

class RigidBody;

class PbdCollisionSdfMeshTreeNode: public PbdCollisionObjectNode
{
    GDCLASS( PbdCollisionSdfMeshTreeNode, PbdCollisionObjectNode );
protected:
    static void _bind_methods();
    void _notification( int p_what );
public:
    PbdCollisionSdfMeshTreeNode();
    ~PbdCollisionSdfMeshTreeNode();

    void set_min_depth( int d );
    int  get_min_depth() const;
    void set_max_depth( int d );
    int  get_max_depth() const;
    void set_max_error( real_t err );
    real_t get_max_error() const;
    void set_min_points( int qty );
    int  get_min_points() const;

    void subdivide();

    PoolVector3Array lines_nodes_sdf();
    PoolVector3Array lines_nodes_pts();
    PoolVector3Array lines_pts();

public:
    NarrowTree tree;
    RigidBody * rigid_body;

private:
    void parse_children();
};



}






#endif





