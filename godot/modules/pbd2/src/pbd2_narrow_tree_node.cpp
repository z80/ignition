
#include "pbd2_narrow_tree_node.h"
#include "pbd2_narrow_tree.h"

namespace Pbd
{

NarrowTreeNode::NarrowTreeNode()
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

NarrowTreeNode::~NarrowTreeNode()
{

}

NarrowTreeNode::NarrowTreeNode( const NarrowTreeNode & inst )
{
    *this = inst;
}

const NarrowTreeNode & NarrowTreeNode::operator=( const NarrowTreeNode & inst )
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
        se3_optimized_  = inst.se3_optimized_;
        cube_optimized_ = inst.cube_optimized_;

        ptInds = inst.ptInds;

        init();
    }

    return *this;
}


void NarrowTreeNode::apply( const SE3 & se3 )
{
    cube_.apply( se3 );
    if ( value > 0 )
    {
        cube_optimized_.apply( se3 * se3_optimized_ );

        const int qty = ptInds.size();
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds.ptr()[i];
            Face & face = tree->faces_.ptrw()[ind];
            face.apply( se3 );
        }
    }
}



bool NarrowTreeNode::hasChildren() const
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

bool NarrowTreeNode::subdivide()
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
    const Float chSize2  = this->size2 * 0.5;

    NarrowTreeNode nn[8];
    int    qtys[8];
    NarrowTreeNode n;
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
    nn[0].center.x_ -= chSize2;
    nn[0].center.y_ -= chSize2;
    nn[0].center.z_ -= chSize2;

    nn[1].center = this->center;
    nn[1].center.x_ += chSize2;
    nn[1].center.y_ -= chSize2;
    nn[1].center.z_ -= chSize2;

    nn[2].center = this->center;
    nn[2].center.x_ -= chSize2;
    nn[2].center.y_ += chSize2;
    nn[2].center.z_ -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x_ += chSize2;
    nn[3].center.y_ += chSize2;
    nn[3].center.z_ -= chSize2;

    nn[4].center = this->center;
    nn[4].center.x_ -= chSize2;
    nn[4].center.y_ -= chSize2;
    nn[4].center.z_ += chSize2;

    nn[5].center = this->center;
    nn[5].center.x_ += chSize2;
    nn[5].center.y_ -= chSize2;
    nn[5].center.z_ += chSize2;

    nn[6].center = this->center;
    nn[6].center.x_ -= chSize2;
    nn[6].center.y_ += chSize2;
    nn[6].center.z_ += chSize2;

    nn[7].center = this->center;
    nn[7].center.x_ += chSize2;
    nn[7].center.y_ += chSize2;
    nn[7].center.z_ += chSize2;

    for ( int i=0; i<8; i++ )
        nn[i].init();

    // Need to assign center and size
    const int qty = ptInds.size();
    for ( int j=0; j<8; j++ )
    {
        NarrowTreeNode & ch_n = nn[j];
        ch_n.value = 0;
        for ( int i=0; i<qty; i++ )
        {
            const int ind = ptInds[i];
            const Face & face = tree->faces_[ind];

            const bool inside = ch_n.inside( face );
            if ( inside )
            {
                ch_n.ptInds.push_back( ind );
                qtys[j] += 1;
                ch_n.value += 1;
            }
        }
        // This is probably debug code as it duplicates stuff above.
        if ( qtys[j] == 0 )
        {
           for ( int i=0; i<qty; i++ )
           {
               const int ind = ptInds[i];
               const Face & face = tree->faces_[ind];

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
        NarrowTreeNode & ch_n = nn[i];
        if ( ( qtys[i] > 0 ) && ( childLevel < tree->max_depth_ ) )
            ch_n.subdivide();
        tree->nodes_.ptrw()[ ch_n.absIndex ] = ch_n;
    }

    return true;
}


bool NarrowTreeNode::inside( const Face & face ) const
{
    const bool intersects = cube_.intersects( face );
    return intersects;
}

bool NarrowTreeNode::inside( const NarrowTreeNode & n ) const
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
        const NarrowTreeNode & ch_n = tree->nodes_.ptr()[ind];
        const bool ch_intersects = inside( ch_n );
        if ( ch_intersects )
            return true;
    }

    return false;
}

void NarrowTreeNode::init()
{
    cube_.init( center, size2, size2, size2 );
}




bool NarrowTreeNode::collide_forward( const NarrowTreeNode & n, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    const bool intersects = cube_.intersects( n.cube_ );
    if ( !intersects )
        return false;
    const bool has_ch = n.hasChildren();
    if ( !has_ch )
    {
        const bool is_filled = (n.value > 0);
        if ( !is_filled )
            return false;
        const bool ret = n.collide_backward( *this, pts, depths );
        return ret;
    }

    bool children_intersect = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = n.children[i];
        const NarrowTreeNode & child_node = tree->nodes_.ptr()[ind];
        const bool ch_intersects = collide_forward( child_node, pts, depths );
        children_intersect = children_intersect || ch_intersects;
    }

    return children_intersect;
}

bool NarrowTreeNode::collide_backward( const NarrowTreeNode & this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    const bool intersects = cube_.intersects( this_node.cube_ );
    if ( !intersects )
        return false;
    const bool has_ch = this_node.hasChildren();
    if ( !has_ch )
    {
        const bool is_filled = (this_node.value > 0);
        if ( !is_filled )
            return false;
        // Colliding individual faces.
        const bool ret = collide_faces( this_node, pts, depths );
        return ret;
    }

    bool children_intersect = false;
    for ( int i=0; i<8; i++ )
    {
        const int ind = this_node.children[i];
        const NarrowTreeNode & child_node = tree->nodes_.ptr()[ind];
        const bool ch_intersects = collide_backward( child_node, pts, depths );
        children_intersect = children_intersect || ch_intersects;
    }

    return children_intersect;

    return true;
}


bool NarrowTreeNode::collide_faces( const NarrowTreeNode & this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const
{
    const NarrowTreeNode & other_node = *this;
    const int own_qty   = this_node.ptInds.size();
    const int other_qty = other_node.ptInds.size();
    Vector3d face_pts[3];
    Vector3d face_depths[3];
    
    bool ret = false;
    for ( int i=0; i<own_qty; i++ )
    {
        const int this_ind = this_node.ptInds.ptr()[i];
        const Face & this_face = tree->faces_.ptr()[this_ind];
        for ( int j=0; j<other_qty; j++ )
        {
            const int other_ind = other_node.ptInds.ptr()[j];
            const Face & other_face = tree->faces_.ptr()[other_ind];
            const int qty = this_face.intersects_all( other_face, face_pts, face_depths );
            ret = ret || (qty > 0);
            for ( int k=0; k<qty; k++ )
            {
                const Vector3d & pt    = face_pts[k];
                const Vector3d & depth = face_depths[k];
                pts.push_back( pt );
                depths.push_back( depth );
            }
        }
    }
    return ret;
}







}



