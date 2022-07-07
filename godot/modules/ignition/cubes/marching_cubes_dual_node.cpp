
#include "marching_cubes_dual_node.h"
#include "marching_cubes_dual.h"

namespace Ign
{

MarchingCubesDualNode::MarchingCubesDualNode()
{
    self_index      = -1;
    index_in_parent = -1;
    parent_node     = nullptr;
    for ( int i=0; i<8; i++ )
        child_nodes[i] = nullptr;
    size   = -1;

    for ( int i=0; i<8; i++ )
        values[i] = 0.0;
}

MarchingCubesDualNode::~MarchingCubesDualNode()
{
}

MarchingCubesDualNode::MarchingCubesDualNode( const MarchingCubesDualNode & inst )
{
    *this = inst;
}

const MarchingCubesDualNode & MarchingCubesDualNode::operator=( const MarchingCubesDualNode & inst )
{
    if ( this != &inst )
    {
        self_index      = inst.self_index;
        index_in_parent = inst.index_in_parent;
        parent_node     = inst.parent_node;
        for ( int i=0; i<8; i++ )
            child_nodes[i] = inst.child_nodes[i];
        size   = inst.size;
        at     = inst.at;

        for ( int i=0; i<8; i++ )
        {
            vertices_int[i] = inst.vertices_int[i];
            vertices_scaled[i]     = inst.vertices_scaled[i];
            vertices[i] = inst.vertices[i];
            values[i]       = inst.values[i];
        }
    }

    return *this;
}

bool MarchingCubesDualNode::has_children() const
{
    for ( int i=0; i<8; i++ )
    {
        const bool has = (child_nodes[i] != nullptr);
        if ( has )
            return true;
    }

    return false;
}

bool MarchingCubesDualNode::subdivide( MarchingCubesDual * tree, VolumeSource * source, const DistanceScalerBase * scaler )
{
    if ( size <= 2 )
        return false;

    const bool should_split = tree->should_split( this, source, scaler );
    if ( !should_split )
        return false;

    const int size_2 = size / 2;

    for ( int i=0; i<8; i++ )
    {
        child_nodes[i]                  = tree->create_node();
        child_nodes[i]->size            = size_2;
        child_nodes[i]->parent_node     = this;
        child_nodes[i]->index_in_parent = i;
    }

    child_nodes[0]->at = this->at;
    child_nodes[1]->at = this->at + VectorInt( size_2,      0,      0 );
    child_nodes[2]->at = this->at + VectorInt( size_2,      0, size_2 );
    child_nodes[3]->at = this->at + VectorInt(      0,      0, size_2 );
    child_nodes[4]->at = this->at + VectorInt(      0, size_2,      0 );
    child_nodes[5]->at = this->at + VectorInt( size_2, size_2,      0 );
    child_nodes[6]->at = this->at + VectorInt( size_2, size_2, size_2 );
    child_nodes[7]->at = this->at + VectorInt(      0, size_2, size_2 );

    for ( int i=0; i<8; i++ )
		tree->compute_node_values( *(child_nodes[i]), source, scaler );

	for ( int i=0; i<8; i++ )
		child_nodes[i]->subdivide( tree, source, scaler );

    return true;
}

void MarchingCubesDualNode::init( MarchingCubesDual * tree )
{
}


const VectorInt MarchingCubesDualNode::center() const
{
    const int size_2 = size / 2;
    const VectorInt c = at + VectorInt( size_2, size_2, size_2 );
    return c;
}

bool MarchingCubesDualNode::has_surface() const
{
    int above_qty = 0;
    int below_qty = 0;
    for ( int i=0; i<8; i++ )
    {
        const Float v = values[i];
        if ( v > 0.0 )
            above_qty += 1;
        else //if ( v <= 0.0 )
            below_qty += 1;

        if ( (above_qty > 0) && (below_qty > 0) )
            return true;
    }

    return false;
}

bool MarchingCubesDualNode::at_left( const MarchingCubesDualNode * root ) const
{
    const bool ret = (at.x <= root->at.x);
    return ret;
}

bool MarchingCubesDualNode::at_right( const MarchingCubesDualNode * root ) const
{
    const bool ret = ((at.x+size) >= (root->at.x+root->size));
    return ret;
}

bool MarchingCubesDualNode::at_bottom( const MarchingCubesDualNode * root ) const
{
    const bool ret = (at.y <= root->at.y);
    return ret;
}

bool MarchingCubesDualNode::at_top( const MarchingCubesDualNode * root ) const
{
    const bool ret = ((at.y+size) >= (root->at.y+root->size));
    return ret;
}

bool MarchingCubesDualNode::at_back( const MarchingCubesDualNode * root ) const
{
    const bool ret = (at.z <= root->at.z);
    return ret;
}

bool MarchingCubesDualNode::at_front( const MarchingCubesDualNode * root ) const
{
    const bool ret = ((at.z+size) >= (root->at.z+root->size));
    return ret;
}














}



