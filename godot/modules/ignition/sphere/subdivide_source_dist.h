
#ifndef __SUBDIVIDE_SOURCE_DIST_H_
#define __SUBDIVIDE_SOURCE_DIST_H_

#include "subdivide_source.h"

namespace Ign
{

class CubeQuadNode;
class CubeSphere;

class SubdivideSourceDist: public SubdivideSource
{
public:

    SubdivideSourceDist();
    virtual ~SubdivideSourceDist();

    // Based on changed interest points determines if 
    // need to recomute subdivision. 
    virtual bool need_subdivide( const CubeSphere * s, const Vector<Vector3d> & pts ) override;
    // Determines if a particular face should be subdivided.
    virtual bool need_subdivide( const CubeSphere * s, const CubeQuadNode * f ) const override;

public:
    Float min_size;
    Float min_angle;
};




}





#endif




