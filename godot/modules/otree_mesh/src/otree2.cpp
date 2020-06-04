
#include "otree2.h"
#include "config_reader.h"

namespace KEYPOINT_BA
{

OTree2::OTree2( int maxPtsPerNode, int maxLvl )
{
    // Initialize counters and parameters.
    this->maxDepth      = maxLvl;
    this->maxPtsPerNode = maxPtsPerNode;

    // Insert root node.
    /*
    ONode2 r;
    r.absIndex = rootIndex;
    r.level = 0;
    r.size2 = 1.0;
    r.tree  = this;
    */
}

OTree2::OTree2( KEYPOINT_BA::ConfigReader * cr )
{
    this->maxDepth      = 7;
    this->maxPtsPerNode = 10;
    if ( cr )
    {
        cr->readInt( "frameDictOptimizer.OTree2.maxPtsPerNode", maxPtsPerNode, maxPtsPerNode );
        cr->readInt( "frameDictOptimizer.OTree2.maxDepth",      maxDepth,      maxDepth );
    }
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
        ptRefs        = inst.ptRefs;
        nodes         = inst.nodes;

        maxDepth      = inst.maxDepth;
        maxPtsPerNode = inst.maxPtsPerNode;
    }
    return *this;
}

void OTree2::operator=( const Keypoint3d::Map & pts3 )
{
    nodes.clear();
    ptRefs.clear();
    if ( pts3.empty() )
        return;

    const Point3d & p = pts3.begin()->second.at;
    Float minX = p.x(),
          maxX = p.x();
    Float minY = p.y(),
          maxY = p.y();
    Float minZ = p.z(),
          maxZ = p.z();

    // First compute bounding box for all 3d points.
    int qty = 0;
    for ( Keypoint3d::Map::const_iterator it=pts3.begin();
          it!=pts3.end(); it++ )
    {
        const Point3d & r = it->second.at;
        if ( minX > r.x() )
            minX = r.x();
        if ( maxX < r.x() )
            maxX = r.x();
        if ( minY > r.y() )
            minY = r.y();
        if ( maxY < r.y() )
            maxY = r.y();
        if ( minZ > r.z() )
            minZ = r.z();
        if ( maxZ < r.z() )
            maxZ = r.z();

        qty += 1;
    }
    const Point3d c( (maxX+minX)*0.5, (maxY+minY)*0.5, (maxZ+minZ)*0.5 );
    const Float dx = maxX - minX;
    const Float dy = maxY - minY;
    const Float dz = maxZ - minZ;
    const Float d = std::max( dx, std::max( dy, dz ) );

    // Insert points into the tree.
    ptRefs.reserve( qty );
    PtRef2 ptRef;
    for ( Keypoint3d::Map::const_iterator it=pts3.begin();
          it!=pts3.end(); it++ )
    {
        ptRef.id = it->first;
        ptRef.pt3 = it->second.at;
        ptRefs.push_back( ptRef );
    }


    ONode2 r;
    r.level = 0;
    r.center = c;
    r.size2 = d;
    r.tree  = this;
    nodes.push_back( r );

    ONode2 & n = nodes[0];
    r.ptInds.reserve( qty );
    for ( int i=0; i<qty; i++ )
        r.ptInds.push_back( i );

    if ( qty > maxPtsPerNode )
        r.subdrive();
    nodes[0] = r;
}

void OTree2::visiblePts3( const Camera & c, std::vector<int> & ptInds ) const
{
    ptInds.clear();
    if ( nodes.empty() )
        return;
    const ONode2 & n = nodes[0];
    addNodePts3( c, n, ptInds );
}

void OTree2::addNodePts3( const Camera & c, const ONode2 & n, std::vector<int> & ptInds ) const
{
    if ( nodeInvisible( c, n ) )
        return;
    if ( n.hasChildren() )
    {
        for ( int i=0; i<8; i++ )
        {
            const int chId = n.children[i];
            const ONode2 & cn = nodes[chId];
            addNodePts3( c, cn, ptInds );
        }
        return;
    }
    if ( !n.ptInds.empty() )
    {
        for ( std::vector<int>::const_iterator it=n.ptInds.begin();
              it!=n.ptInds.end(); it++ )
        {
            const int ind = *it;
            const int pt3Id = ptRefs[ind].id;
            ptInds.push_back( pt3Id );
        }
    }
}

bool OTree2::parent( const ONode2 & node, ONode2 * & parent )
{
    if ( node.parentAbsIndex < 0 )
    {
        parent = 0;
        return false;
    }

    parent = &( nodes[ node.parentAbsIndex ] );
    return true;
}

int OTree2::insertNode( ONode2 & node )
{
    nodes.push_back( node );
    const int ind = static_cast<int>(nodes.size()) - 1;
    nodes[ind].tree     = this;
    nodes[ind].absIndex = ind;
    return ind;
}

void OTree2::updateNode( const ONode2 & node )
{
    nodes[ node.absIndex ] = node;
}

bool OTree2::nodeInvisible( const Camera & c, const ONode2 & n ) const
{
    Point3d v[8];
    n.vertices( v );
    for ( int i=0; i<6; i++ )
    {
        bool allInFront = true;
        for ( int j=0; j<8; j++ )
        {
            const bool res = c.planes[i].inFront( v[j] );
            allInFront = allInFront && res;
            if ( !res )
                break;
        }
        // All 8 vertices are in front of one of planes.
        // It means that node is invisible.
        if ( allInFront )
            return true;
    }

    // For none of 6 planes all 8 vertices in front.
    // It meant the node may not be invisible.
    return false;
}

}




