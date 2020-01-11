
#ifndef __CUBE_TREE_H_
#define __CUBE_TREE_H_

#include "Urho3D/Urho3DAll.h"
#include "vector3d.h"

namespace Ign
{

class CubeTree;



class Plane
{
public:
    Vector3d r;
    Vector3d a;

    Plane();
    ~Plane();
    Plane( const Plane & inst );
    const Plane & operator=( const Plane & inst );
    bool inFront( const Vector3d & t ) const;
};

class CubeTreeNode
{
public:
    CubeTreeNode();
    ~CubeTreeNode();
    CubeTreeNode( const CubeTreeNode & inst );
    const CubeTreeNode & operator=( const CubeTreeNode & inst );

    bool inside( const Vector3d & pt ) const;
    bool hasChildren() const;
    bool subdrive();

    void vertices( Vector3d * verts ) const;
    void planes( Plane * planes ) const;
    //bool intersectsRay( const Vector3d & start, const Vector3d & end ) const;

    CubeTree * tree;
    int absIndex;
    int parentAbsIndex;
    int indexInParent;
    int level;

    // Child indices in OTree dictionary.
    int children[8];

    Float    value;
    Float    size2; // Size over 2.
    Vector3d center;

    Vector<int> ptInds;
};

class CubeTree
{
public:
    CubeTree( int maxPtsPerNode=25, int maxLvl=30 );
    ~CubeTree();

    CubeTree( const CubeTree & inst );
    const CubeTree & operator=( const CubeTree & inst );

    //void operator=( const Keypoint3d::Map & pts3 );
    //void visiblePts3( const Camera & c, std::vector<int> & ptInds ) const;
    //void addNodePts3( const Camera & c, const ONode2 & n, std::vector<int> & ptInds ) const;

    bool parent( const CubeTreeNode & node, CubeTreeNode * & parent );

    int  insertNode( CubeTreeNode & node );
    void updateNode( const CubeTreeNode & node );

    bool nodeInvisible( const Vector3d & c, const CubeTreeNode & n ) const;

    Vector<CubeTreeNode> nodes;
    Vector<Vector3d>     pts3d;

    // Maximum subdivision level.
    int maxDepth;
    int maxPtsPerNode;
};

}


#endif



