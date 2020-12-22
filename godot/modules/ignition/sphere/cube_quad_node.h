
#ifndef __CUBE_QUAD_NODE_H_
#define __CUBE_QUAD_NODE_H_

#include "vector3d.h"
#include "cube_hash.h"

namespace Ign
{

class SubdivideSource;
class CubeSphere;

class CubeQuadNode
{
public:
    int  vertexInds[4];
    int  childInds[4];
    int  parentInd;
    int  indexInParent;
    int  level;
    bool leaf;
    SphereHash hash_;

    CubeQuadNode();
    ~CubeQuadNode();


    CubeQuadNode( int a, int b, int c, int d );
    CubeQuadNode( const CubeQuadNode & inst );
    const CubeQuadNode & operator=( const CubeQuadNode & inst );
    bool subdivide( CubeSphere * s, SubdivideSource * src );

    const Vector3d normal( const CubeSphere * s ) const;
    const Float size( const CubeSphere * s ) const;
    static bool correct_side( const Vector3d & n, const Vector3d & a );
    // From 3d space to a face of a unit cube.
    static bool central_projection( const Vector3d & n, const Vector3d & a, Vector3d & proj );
    // Vector "a" should be already projected on appropriate face.
    // E.i. "correctSide()" and "centralProjection()" should be already done.
    // "sz" allows to select all the faces within an area of size "sz".
    bool inside( const CubeSphere * s, const Vector3d & a, const Vector3d & n, const Float dist=0.1 ) const;
    bool select_leafs( const CubeSphere * s, const Vector3d & a, const Float dist, Vector<int> & faceInds ) const;
    bool select_by_size( const CubeSphere * s, const Vector3d & a, const Float sz, const Float dist, Vector<int> & faceInds ) const;
};





}





#endif





