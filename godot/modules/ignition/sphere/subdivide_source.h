
#ifndef __SUBDIVIDE_SOURCE_H_
#define __SUBDIVIDE_SOURCE_H_

#include "vector3d.h"

namespace Ign
{

class CubeQuadNode;
class CubeSphere;

class SubdivideSource
{
public:

    SubdivideSource();
    virtual ~SubdivideSource();

    // Based on changed interest points determines if 
    // need to recomute subdivision. 
    virtual bool need_subdivide( const CubeSphere * s, const Vector<Vector3d> & pts );
    // Determines if a particular face should be subdivided.
    virtual bool need_subdivide( const CubeSphere * s, const CubeQuadNode * f ) const;

public:
    void flatten_pts( const CubeSphere * s );

    Vector<Vector3d> pts_, ptsFlat_, ptsNew_;
};




}





#endif




