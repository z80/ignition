
#include "otree2.h"

namespace KEYPOINT_BA
{

ONode2::ONode2()
{
    this->tree = 0;
    parentAbsIndex = -1;
    indexInParent = -1;
    level = -1;
    absIndex = -1;

    for ( int i=0; i<8; i++ )
        children[i] = -1;

    size2 = 1.0;
    center = Point3d( 0.0, 0.0, 0.0 );
}

ONode2::~ONode2()
{

}

ONode2::ONode2( const ONode2 & inst )
{
    *this = inst;
}

const ONode2 & ONode2::operator=( const ONode2 & inst )
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

bool ONode2::inside( const Point3d & pt ) const
{
    if ( pt.x() > ( center.x() + size2 ) )
        return false;
    if ( pt.x() < ( center.x() - size2 ) )
        return false;

    if ( pt.y() > ( center.y() + size2 ) )
        return false;
    if ( pt.y() < ( center.y() - size2 ) )
        return false;

    if ( pt.z() > ( center.z() + size2 ) )
        return false;
    if ( pt.z() < ( center.z() - size2 ) )
        return false;

    return true;
}

bool ONode2::hasChildren() const
{
    // Filled can't have children.
    if ( !ptInds.empty() )
        return false;

    for ( int i=0; i<8; i++ )
    {
        if ( children[i] > 0 )
            return true;
    }

    return false;
}

bool ONode2::subdrive()
{
    // Subdrive means creating children.
    // It means node itself turns into not filled one.

    if ( level >= tree->maxDepth )
        return false;

    bool ch = hasChildren();
    if ( ch )
        return false;

    if ( ptInds.empty() )
        return false;

    const int childLevel = this->level + 1;
    const Float chSize2 = this->size2 * 0.5;

    ONode2 nn[8];
    int    qtys[8];
    ONode2 n;
    for ( int i=0; i<8; i++ )
    {
        n.absIndex = tree->insertNode( n );
        nn[i] = tree->nodes[ n.absIndex ];

        qtys[i] = 0;
    }

    const int expectedQty = static_cast<int>( ptInds.size() ) / 8;
    for ( int i=0; i<8; i++ )
    {
        nn[i].indexInParent = i;
        nn[i].parentAbsIndex = this->absIndex;
        nn[i].level = childLevel;
        nn[i].size2 = chSize2;

        children[i] = nn[i].absIndex;

        // Reserve memory for faster operations.
        nn[i].ptInds.reserve( expectedQty );
    }
    nn[0].center = this->center;
    nn[0].center.x() -= chSize2;
    nn[0].center.y() -= chSize2;
    nn[0].center.z() -= chSize2;

    nn[1].center = this->center;
    nn[1].center.x() += chSize2;
    nn[1].center.y() -= chSize2;
    nn[1].center.z() -= chSize2;

    nn[2].center = this->center;
    nn[2].center.x() -= chSize2;
    nn[2].center.y() += chSize2;
    nn[2].center.z() -= chSize2;

    nn[3].center = this->center;
    nn[3].center.x() += chSize2;
    nn[3].center.y() += chSize2;
    nn[3].center.z() -= chSize2;

    nn[4].center = this->center;
    nn[4].center.x() -= chSize2;
    nn[4].center.y() -= chSize2;
    nn[4].center.z() += chSize2;

    nn[5].center = this->center;
    nn[5].center.x() += chSize2;
    nn[5].center.y() -= chSize2;
    nn[5].center.z() += chSize2;

    nn[6].center = this->center;
    nn[6].center.x() -= chSize2;
    nn[6].center.y() += chSize2;
    nn[6].center.z() += chSize2;

    nn[7].center = this->center;
    nn[7].center.x() += chSize2;
    nn[7].center.y() += chSize2;
    nn[7].center.z() += chSize2;

    // Need to assign center and size
    for ( std::vector<int>::const_iterator it=ptInds.begin();
          it!=ptInds.end(); it++ )
    {
        const int ind = *it;
        const PtRef2 & ptRef = tree->ptRefs[ind];
        const int pt3Id      = ptRef.id;
        const Point3d & pt3  = ptRef.pt3;

        for ( int i=0; i<8; i++ )
        {
            if ( nn[i].inside( pt3 ) )
            {
                nn[i].ptInds.push_back( ind );
                qtys[i] += 1;
                break;
            }
        }

    }
    ptInds.clear();

    for ( int i=0; i<8; i++ )
    {
        if ( qtys[i] > tree->maxPtsPerNode )
            nn[i].subdrive();
        tree->nodes[ nn[i].absIndex ] = nn[i];
    }

    return true;
}

void ONode2::planes( Plane * planes ) const
{
    // Front.
    planes[0].r = center;
    planes[0].r.y() += size2;
    planes[0].a = Point3d( 0.0, 1.0, 0.0 );

    // Back
    planes[1].r = center;
    planes[1].r.y() -= size2;
    planes[1].a = Point3d( 0.0, -1.0, 0.0 );

    // Left
    planes[2].r = center;
    planes[2].r.x() -= size2;
    planes[2].a = Point3d( -1.0, 0.0, 0.0 );

    // Right
    planes[3].r = center;
    planes[3].r.x() += size2;
    planes[3].a = Point3d( 1.0, 0.0, 0.0 );

    // Top
    planes[4].r = center;
    planes[4].r.z() += size2;
    planes[4].a = Point3d( 0.0, 0.0, 1.0 );

    // Bottom
    planes[5].r = center;
    planes[5].r.z() -= size2;
    planes[5].a = Point3d( 0.0, 0.0, -1.0 );
}

void ONode2::vertices( Point3d * verts ) const
{
    verts[0].x() = center.x() - size2;
    verts[0].y() = center.y() - size2;
    verts[0].z() = center.z() - size2;

    verts[1].x() = center.x() + size2;
    verts[1].y() = center.y() - size2;
    verts[1].z() = center.z() - size2;

    verts[2].x() = center.x() - size2;
    verts[2].y() = center.y() + size2;
    verts[2].z() = center.z() - size2;

    verts[3].x() = center.x() + size2;
    verts[3].y() = center.y() + size2;
    verts[3].z() = center.z() - size2;

    verts[4].x() = center.x() - size2;
    verts[4].y() = center.y() - size2;
    verts[4].z() = center.z() + size2;

    verts[5].x() = center.x() + size2;
    verts[5].y() = center.y() - size2;
    verts[5].z() = center.z() + size2;

    verts[6].x() = center.x() - size2;
    verts[6].y() = center.y() + size2;
    verts[6].z() = center.z() + size2;

    verts[7].x() = center.x() + size2;
    verts[7].y() = center.y() + size2;
    verts[7].z() = center.z() + size2;
}



}


