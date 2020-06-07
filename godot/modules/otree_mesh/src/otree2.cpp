
#include "otree2.h"



OTree2::OTree2( real_t node_sz )
{
    // Initialize counters and parameters.
    this->max_depth_  = -1;
    this->node_sz_    = node_sz;

    // Insert root node.
    /*
    ONode2 r;
    r.absIndex = rootIndex;
    r.level = 0;
    r.size2 = 1.0;
    r.tree  = this;
    */
}

OTree2::~OTree2()
{

}

OTree2::OTree2( const OTree2 & inst )
{
    *this = inst;
}

const OTree2 & OTree2::operator=( const OTree2 & inst )
{
    if ( this != &inst )
    {
        ptRefs     = inst.ptRefs;
        nodes      = inst.nodes;

		node_sz_   = inst.node_sz_;
        max_depth_ = inst.max_depth_;
    }
    return *this;
}

bool OTree2::parent( const ONode2 & node, ONode2 * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes.ptrw()[ node.parentAbsIndex ] );
    return true;
}

int OTree2::insertNode( ONode2 & node )
{
    nodes.push_back( node );
    const int ind = static_cast<int>(nodes.size()) - 1;
	ONode2 * nns = nodes.ptrw();
	ONode2 & n = nns[ind];
    n.tree     = this;
    n.absIndex = ind;
    return ind;
}

void OTree2::updateNode( const ONode2 & node )
{
    nodes.ptrw()[ node.absIndex ] = node;
}






