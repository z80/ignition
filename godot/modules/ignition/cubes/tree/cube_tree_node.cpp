
#include "cube_tree_node.h"
#include "cube_tree.h"
#include "marching_volume_object.h"

#include <algorithm>

namespace Ign
{

CubeTreeNode::CubeTreeNode()
{
    parent_abs_index = -1;
    index_in_parent  = -1;
    abs_index        = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    size   = 1;
    corner = VectorInt( 0, 0, 0 );
}

CubeTreeNode::~CubeTreeNode()
{
}

CubeTreeNode::CubeTreeNode( const CubeTreeNode & inst )
{
    *this = inst;
}

const CubeTreeNode & CubeTreeNode::operator=( const CubeTreeNode & inst )
{
    if ( this != &inst )
    {
        parent_abs_index = inst.parent_abs_index;
        index_in_parent  = inst.index_in_parent;
        abs_index        = inst.abs_index;

        children[0] = inst.children[0];
        children[1] = inst.children[1];
        children[2] = inst.children[2];
        children[3] = inst.children[3];
        children[4] = inst.children[4];
        children[5] = inst.children[5];
        children[6] = inst.children[6];
        children[7] = inst.children[7];

        size   = inst.size;
        corner = inst.corner;

        source_inds = inst.source_inds;

        corner_min = inst.corner_min;
        corner_max = inst.corner_max;
    }

    return *this;
}

bool CubeTreeNode::has_children() const
{
    // Filled can't have children.
    if ( !source_inds.empty() )
        return false;

    for ( int i=0; i<8; i++ )
    {
        if ( children[i] >= 0 )
            return true;
    }

    return false;
}

bool CubeTreeNode::subdivide( CubeTree * tree )
{
    if (size <= 1)
    {
        return false;
    }

    // It can't have children because subdivision is happening.
    // But just in case.
    bool ch = has_children();
    if ( ch )
        return false;

    // If it is empty, no need to subdivide.
    // Or if there is just one object inside, also don't subdivide.
    if ( source_inds.size() <= tree->max_items_per_node )
        return false;

    const Float ch_size = this->size / 2;

    CubeTreeNode nn[8];
    int    qtys[8];
    CubeTreeNode n;
    for ( int i=0; i<8; i++ )
    {
        n.abs_index = tree->insert_node( n );
        nn[i] = tree->nodes[ n.abs_index ];

        nn[i].index_in_parent = i;
        nn[i].parent_abs_index = this->abs_index;
        nn[i].size = ch_size;

        children[i] = nn[i].abs_index;

        qtys[i] = 0;
    }
    nn[0].corner = this->corner;

    nn[1].corner = this->corner;
    nn[1].corner.x += ch_size;

    nn[2].corner = this->corner;
    nn[2].corner.x += ch_size;
    nn[2].corner.y += ch_size;

    nn[3].corner = this->corner;
    nn[3].corner.y += ch_size;

    nn[4].corner = this->corner;
    nn[4].corner.z += ch_size;

    nn[5].corner = this->corner;
    nn[5].corner.x += ch_size;
    nn[5].corner.z += ch_size;

    nn[6].corner = this->corner;
    nn[6].corner.x += ch_size;
    nn[6].corner.y += ch_size;
    nn[6].corner.z += ch_size;

    nn[7].corner = this->corner;
    nn[7].corner.y += ch_size;
    nn[7].corner.z += ch_size;

    for ( int i=0; i<8; i++ )
        nn[i].init( tree );

    // Need to assign center and size
    const int qty = source_inds.size();
    for ( int j=0; j<8; j++ )
    {
        CubeTreeNode & ch_n = nn[j];
        for ( int i=0; i<qty; i++ )
        {
            const int ind = source_inds[i];
            MarchingVolumeObject * source = tree->sources[ind];

            const bool inside = source->inside( &ch_n );
            if ( inside )
            {
                ch_n.source_inds.push_back( ind );
                qtys[j] += 1;
            }
        }
    }

    // Reset indices and value for a node with children.
    source_inds.clear();

    // Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        CubeTreeNode & ch_n = nn[i];
        ch_n.subdivide( tree );
        tree->update_node( ch_n );
    }

    return true;
}


void CubeTreeNode::init( CubeTree * tree )
{
    corner_min = tree->at( corner );
    corner_max = tree->at( VectorInt( corner.x + size, corner.y + size, corner.z + size ) );
}


bool CubeTreeNode::contains_point( const Vector3d & at ) const
{
    if ( at.x_ < corner_min.x_ )
        return false;
    if ( at.y_ < corner_min.y_ )
        return false;
    if ( at.z_ < corner_min.z_ )
        return false;

    if ( at.x_ > corner_max.x_ )
        return false;
    if ( at.y_ > corner_max.y_ )
        return false;
    if ( at.z_ > corner_max.z_ )
        return false;

    return true;
}

void CubeTreeNode::pick_objects( CubeTree * tree, std::vector<int> & query )
{
    const bool is_leaf = has_children();
    if ( is_leaf )
    {
        const int qty = source_inds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int ind = source_inds[i];
            const bool unique = ( std::find( query.begin(), query.end(), ind ) == query.end() );
            if ( unique )
                query.push_back( ind );
        }
        return;
    }

    for ( int i=0; i<8; i++ )
    {
        const int child_ind = children[i];
        CubeTreeNode & child_node = tree->nodes[child_ind];
        child_node.pick_objects( tree, query );
    }
}




}


