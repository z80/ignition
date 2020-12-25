
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
    struct Level
    {
        Float sz;
        Float dist;
    };
    struct SubdividePoint
    {
        Vector3d at;
        bool     close;
    };

    SubdivideSource();
    virtual ~SubdivideSource();

    void clear_levels();
    void add_level( Float sz, Float dist );

    // Based on changed interest points determines if 
    // need to recomute subdivision. 
    virtual bool need_subdivide( const CubeSphere * s, Vector<SubdividePoint> & pts );
    // Determines if a particular face should be subdivided.
    virtual bool need_subdivide( const CubeSphere * s, const CubeQuadNode * f ) const;

public:
    void flatten_pts( const CubeSphere * s );
    void sort_levels( const CubeSphere * s );

    Vector<Level> levels_, levelsUnit_;
    Vector<SubdividePoint> pts_, ptsFlat_, ptsNew_;
};




}





#endif




