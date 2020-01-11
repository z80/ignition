
#include "cube_tree.h"

namespace Ign
{

Plane::Plane()
{
    a.y_ = 1.0;
}

Plane::~Plane()
{

}

Plane::Plane( const Plane & inst )
{
    *this = inst;
}

const Plane & Plane::operator=( const Plane & inst )
{
    if ( this != &inst )
    {
        r = inst.r;
        a = inst.a;
    }
    return *this;
}

bool Plane::inFront( const Vector3d & t ) const
{
    const Vector3d dr = t - r;
    const Float d = dr.DotProduct( a );
    const bool res = ( d > 0.0 );
    return res;
}











CubeTreeNode::CubeTreeNode()
{
    this->tree = nullptr;
    parentAbsIndex = -1;
    indexInParent = -1;
    level = -1;
    absIndex = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    size2 = 1.0;
    center = Vector3d::ZERO;
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
        tree = inst.tree;
        parentAbsIndex = inst.parentAbsIndex;
        indexInParent = inst.indexInParent;
        level = inst.level;
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

        ptInds = inst.ptInds;
    }

    return *this;
}

bool CubeTreeNode::inside( const Vector3d & pt ) const
{
    if ( pt.x_ > ( center.x_ + size2 ) )
        return false;
    if ( pt.x_ < ( center.x_ - size2 ) )
        return false;

    if ( pt.y_ > ( center.y_ + size2 ) )
        return false;
    if ( pt.y_ < ( center.y_ - size2 ) )
        return false;

    if ( pt.z_ > ( center.z_ + size2 ) )
        return false;
    if ( pt.z_ < ( center.z_ - size2 ) )
        return false;

    return true;
}

bool CubeTreeNode::hasChildren() const
{
    // Filled can't have children.
    if ( !ptInds.Empty() )
        return false;

    for ( int i=0; i<8; i++ )
    {
        if ( children[i] > 0 )
            return true;
    }

    return false;
}

bool CubeTreeNode::subdrive()
{
    // Subdrive means creating children.
    // It means node itself turns into not filled one.

    if ( level >= tree->maxDepth )
        return false;

    bool ch = hasChildren();
    if ( ch )
        return false;

    if ( ptInds.Empty() )
        return false;

    const int childLevel = this->level + 1;
    const Float chSize2 = this->size2 * 0.5;

    CubeTreeNode nn[8];
    int    qtys[8];
    CubeTreeNode n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insertNode( n );
        nn[i] = tree->nodes[ n.absIndex ];

        qtys[i] = 0;
    }

    const int expectedQty = static_cast<int>( ptInds.Size() ) / 8;
    for ( int i=0; i<8; i++ )
    {
        nn[i].indexInParent = i;
        nn[i].parentAbsIndex = this->absIndex;
        nn[i].level = childLevel;
        nn[i].size2 = chSize2;

        children[i] = nn[i].absIndex;

        // Reserve memory for faster operations.
        nn[i].ptInds.Reserve( expectedQty );
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

    // Need to assign center and size
    for ( Vector<unsigned>::ConstIterator it=ptInds.Begin();
          it!=ptInds.End(); it++ )
    {
        const unsigned ind = *it;
        const Vector3d & pt3 = tree->pts3d[ind];

        for ( int i=0; i<8; i++ )
        {
            if ( nn[i].inside( pt3 ) )
            {
                nn[i].ptInds.Push( ind );
                qtys[i] += 1;
                break;
            }
        }

    }
    ptInds.Clear();

    for ( int i=0; i<8; i++ )
    {
        if ( qtys[i] > tree->maxPtsPerNode )
            nn[i].subdrive();
        tree->nodes[ nn[i].absIndex ] = nn[i];
    }

    return true;
}

void CubeTreeNode::vertices( Vector3d * verts ) const
{
    verts[0].x_ = center.x_ - size2;
    verts[0].y_ = center.y_ - size2;
    verts[0].z_ = center.z_ - size2;

    verts[1].x_ = center.x_ + size2;
    verts[1].y_ = center.y_ - size2;
    verts[1].z_ = center.z_ - size2;

    verts[2].x_ = center.x_ - size2;
    verts[2].y_ = center.y_ + size2;
    verts[2].z_ = center.z_ - size2;

    verts[3].x_ = center.x_ + size2;
    verts[3].y_ = center.y_ + size2;
    verts[3].z_ = center.z_ - size2;

    verts[4].x_ = center.x_ - size2;
    verts[4].y_ = center.y_ - size2;
    verts[4].z_ = center.z_ + size2;

    verts[5].x_ = center.x_ + size2;
    verts[5].y_ = center.y_ - size2;
    verts[5].z_ = center.z_ + size2;

    verts[6].x_ = center.x_ - size2;
    verts[6].y_ = center.y_ + size2;
    verts[6].z_ = center.z_ + size2;

    verts[7].x_ = center.x_ + size2;
    verts[7].y_ = center.y_ + size2;
    verts[7].z_ = center.z_ + size2;
}

void CubeTreeNode::planes( Plane * planes ) const
{
    // Front.
    planes[0].r = center;
    planes[0].r.y_ += size2;
    planes[0].a = Vector3d( 0.0, 1.0, 0.0 );

    // Back
    planes[1].r = center;
    planes[1].r.y_ -= size2;
    planes[1].a = Vector3d( 0.0, -1.0, 0.0 );

    // Left
    planes[2].r = center;
    planes[2].r.x_ -= size2;
    planes[2].a = Vector3d( -1.0, 0.0, 0.0 );

    // Right
    planes[3].r = center;
    planes[3].r.x_ += size2;
    planes[3].a = Vector3d( 1.0, 0.0, 0.0 );

    // Top
    planes[4].r = center;
    planes[4].r.z_ += size2;
    planes[4].a = Vector3d( 0.0, 0.0, 1.0 );

    // Bottom
    planes[5].r = center;
    planes[5].r.z_ -= size2;
    planes[5].a = Vector3d( 0.0, 0.0, -1.0 );
}

/*bool CubeTreeNode::intersectsRay( const Vector3d & start, const Vector3d & end ) const
{

}*/
















CubeTree::CubeTree(int maxLvl )
{
    // Initialize counters and parameters.
    maxDepth      = maxLvl;
}

CubeTree::~CubeTree()
{

}

CubeTree::CubeTree( const CubeTree & inst )
{
    *this = inst;
}

const CubeTree & CubeTree::operator=( const CubeTree & inst )
{
    if ( this != &inst )
    {
        pts3d         = inst.pts3d;
        nodes         = inst.nodes;

        maxDepth      = inst.maxDepth;
    }
    return *this;
}

const CubeTree & CubeTree::operator=( const Vector<Vector3d> & pts )
{
    nodes.Clear();
    pts3d = pts;

    Vector3d ptMin = Vector3d::ZERO;
    Vector3d ptMax = Vector3d::ZERO;

    // First compute bounding box for all 3d points.
    const unsigned qty = pts3d.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const Vector3d & r = pts3d[i];
        if ( ptMin.x_ > r.x_ )
            ptMin.x_ = r.x_;
        if ( ptMax.x_ < r.x_ )
            ptMax.x_ = r.x_;
        if ( ptMin.y_ > r.y_ )
            ptMin.y_ = r.y_;
        if ( ptMax.y_ < r.y_ )
            ptMax.y_ = r.y_;
        if ( ptMin.z_ > r.z_ )
            ptMin.z_ = r.z_;
        if ( ptMax.z_ < r.z_ )
            ptMax.z_ = r.z_;
    }

    const Float la = ptMin.Length();
    const Float lb = ptMax.Length();
    const Float s_ = ( la > lb ) ? la : lb;
    const Float s = s_ * 1.1;

    CubeTreeNode r;
    r.level = 0;
    r.center = Vector3d::ZERO;
    r.size2 = s;
    r.tree  = this;
    nodes.Push( r );

    CubeTreeNode & n = nodes[0];
    r.ptInds.Reserve( qty );
    for ( unsigned i=0; i<qty; i++ )
        r.ptInds.Push( i );

    r.subdrive();
    nodes[0] = r;

    return *this;
}

bool CubeTree::parent( const CubeTreeNode & node, CubeTreeNode * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = nullptr;
        return false;
    }

    parent = &( nodes[ node.parentAbsIndex ] );
    return true;
}

int  CubeTree::insertNode( CubeTreeNode & node )
{
    nodes.Push( node );
    const unsigned ind = nodes.Size() - 1;
    nodes[ind].tree     = this;
    nodes[ind].absIndex = static_cast<int>( ind );
    return static_cast<int>( ind );
}

void CubeTree::updateNode( const CubeTreeNode & node )
{
    nodes[node.absIndex] = node;
}

bool CubeTree::nodeInvisible( const Vector3d & c, const CubeTreeNode & n ) const
{
    return false;
}













}


