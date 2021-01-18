
#ifndef __PBD2_NARROW_TREE_NODE_H_
#define __PBD2_NARROW_TREE_NODE_H_

#include "scene/main/node.h"
#include "pbd2_narrow_tree.h"

namespace Pbd
{

class RigidBody;

class PbdNarrowTreeNode: public Node
{
    GDCLASS( PbdNarrowTreeNode, Node );
protected:
    static void _bind_methods();
    void _notification( int p_what );
public:
    PbdNarrowTreeNode();
    ~PbdNarrowTreeNode();

    void set_min_depth( int d );
    int  get_min_depth() const;
    void set_max_depth( int d );
    int  get_max_depth() const;
    void set_max_error( real_t err );
    real_t get_max_error() const;
    void set_min_points( int qty );
    int  get_min_points() const;

    void subdivide();

public:
    NarrowTree tree;
    RigidBody * rigid_body;

private:
    void parse_children();
};



}






#endif





