
#include "occupancy_tree_node.h"
#include "occupancy_tree.h"

namespace Pbd
{

OccupancyTreeNode::OccupancyTreeNode()
{
    this->tree = nullptr;
    parentAbsIndex = -1;
    indexInParent = -1;
    level = -1;
    absIndex = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    size2 = 1.0;
    center = Vector3( 0.0, 0.0, 0.0 );

    init();
}

OccupancyTreeNode::~OccupancyTreeNode()
{

}

OccupancyTreeNode::OccupancyTreeNode( const OccupancyTreeNode & inst )
{
    *this = inst;
}

const OccupancyTreeNode & OccupancyTreeNode::operator=( const OccupancyTreeNode & inst )
{
    if ( this != &inst )
    {
        tree = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent = inst.indexInParent;
        level = inst.level;
        value = inst.value;
        absIndex = inst.absIndex;

        children[0] = inst.children[0];
        children[1] = inst.children[1];
        children[2] = inst.children[2];
        children[3] = inst.children[3];
        children[4] = inst.children[4];
        children[5] = inst.children[5];
        children[6] = inst.children[6];
        children[7] = inst.children[7];

        size2 = inst.size2;
        center = inst.center;
        
        cube_ = inst.cube_;

        ptInds = inst.ptInds;

        init();
    }

    return *this;
}

bool OccupancyTreeNode::hasChildren() const
{
    // Filled can't have children.
    if ( !ptInds.empty() )
        return false;

    for ( int i=0; i<8; i++ )
    {
        if ( children[i] >= 0 )
            return true;
    }

    return false;
}

bool OccupancyTreeNode::subdivide()
{
    // Subdrive means creating children.
    // It means node itself turns into not filled one.

    if ( level >= tree->max_depth_ )
        return false;
    
    // It can't have children because subdivision is happening.
    // But just in case.
    bool ch = hasChildren();
    if ( ch )
        return false;
    
    // If it is empty, no need to subdivide.
    if ( ptInds.empty() )
        return false;
    
    // Don't subdivide if very few triangles.
    if ( ptInds.size() <= tree->min_triangles_ )
        return false;

    const int childLevel = this->level + 1;
    const real_t chSize2 = this->size2 * 0.5;

    OccupancyTreeNode nn[8];
    int    qtys[8];
    OccupancyTreeNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insert_node( n );
        nn[i] = tree->nodes_[ n.absIndex ];

        nn[i].indexInParent = i;
        nn[i].parentAbsIndex = this->absIndex;
        nn[i].level = childLevel;
        nn[i].size2 = chSize2;

        children[i] = nn[i].absIndex;

        qtys[i] = 0;
    }
    nn[0].center = this->center;
    nn[0].center.x -= chSize2;
    nn[0].center.y -= chSize2;
    nn[0].center.z -= chSize2;

    nn[1].center = this->center;
    nn[1].center.x += chSize2;
    nn[1].center.y -= chSize2;
    nn[1].center.z -= chSize2;

    nn[2].center = this->center;
    nn[2].center.x -= chSize2;
    nn[2].center.y += chSize2;
    nn[2].center.z -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x += chSize2;
    nn[3].center.y += chSize2;
    nn[3].center.z -= chSize2;

    nn[4].center = this->center;
    nn[4].center.x -= chSize2;
    nn[4].center.y -= chSize2;
    nn[4].center.z += chSize2;

    nn[5].center = this->center;
    nn[5].center.x += chSize2;
    nn[5].center.y -= chSize2;
    nn[5].center.z += chSize2;

    nn[6].center = this->center;
    nn[6].center.x -= chSize2;
    nn[6].center.y += chSize2;
    nn[6].center.z += chSize2;

    nn[7].center = this->center;
    nn[7].center.x += chSize2;
    nn[7].center.y += chSize2;
    nn[7].center.z += chSize2;

    for ( int i=0; i<8; i++ )
        nn[i].init();

    // Need to assign center and size
    const int qty = ptInds.size();
    for ( int j=0; j<8; j++ )
    {
        OccupancyTreeNode & ch_n = nn[j];
        ch_n.value = 0;
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds[i];
            const Face3 & face = tree->faces_[ind];

            const bool inside = ch_n.inside( face );
            if ( inside )
            {
                ch_n.ptInds.push_back( ind );
                qtys[j] += 1;
                ch_n.value += 1;
            }
        }
        if ( qtys[j] == 0 )
        {
           for ( int i=0; i<qty; i++ )
           {
               const int ind = ptInds[i];
               const Face3 & face = tree->faces_[ind];

               const bool inside = ch_n.inside( face );
               if ( inside )
               {
                   ch_n.ptInds.push_back( ind );
                   qtys[j] += 1;
                   ch_n.value += 1;
               }
           }
        }
    }

    // Reset indices and value for a node with children.
    ptInds.clear();
    value = 0;

    // Subdivide recursively.
    for ( int i=0; i<8; i++ )
    {
        OccupancyTreeNode & ch_n = nn[i];
        if ( ( qtys[i] > 0 ) && ( childLevel < tree->max_depth_ ) )
            ch_n.subdivide();
        tree->nodes_.ptrw()[ ch_n.absIndex ] = ch_n;
    }

    return true;
}


bool OccupancyTreeNode::inside( const Face3 & face ) const
{
    const bool intersects = cube_.intersects( face );
    return intersects;
}

bool OccupancyTreeNode::inside( const OccupancyTreeNode & n ) const
{
    const bool intersects = cube_.intersects( n.cube_ );
    if ( !intersects )
        return false;
    const bool has_ch = n.hasChildren();
    if ( !has_ch )
    {
        const bool is_filled = (n.value > 0);
        return is_filled;
    }

    for ( int i=0; i<8; i++ )
    {
        const int ind = n.children[i];
        const OccupancyTreeNode & ch_n = tree->nodes_.ptr()[ind];
        const bool ch_intersects = inside( ch_n );
        if ( ch_intersects )
            return true;
    }

    return false;
}

void OccupancyTreeNode::init()
{
    cube.init( center, size2, size2, size2 );
}



}



