
#include "subdivide_source.h"
#include "cube_sphere.h"
#include "cube_quad_node.h"

namespace Ign
{

SubdivideSource::SubdivideSource()
{
}

SubdivideSource::~SubdivideSource()
{

}


bool SubdivideSource::need_subdivide( const CubeSphere * s, const Vector<Vector3d> & pts )
{
    if ( pts.empty() )
        return false;

    ptsNew_ = pts;
    const unsigned ptsNewQty = ptsNew_.size();
    for ( unsigned i=0; i<ptsNewQty; i++ )
    {
        Vector3d & v = ptsNew_.ptrw()[i];
        v.Normalize();
    }

    if ( s->levels_.empty() )
    {
        pts_ = ptsNew_;
        flatten_pts( s );
        return true;
    }

    // Sort and normalize all levels.
    // LEvels are sorted in increasing size order.
    const int levels_qty = s->levelsUnit_.size();
    const CubeSphere::Level lvl_close = s->levelsUnit_.ptr()[0];
    const Float d_close = lvl_close.dist * 0.5;

    // Check all distances. And resubdrive if shifted half the finest distance.
    const unsigned ptsQty = pts_.size();
    bool needSubdrive = false;
    if ( ptsNewQty > 0 )
    {
        if ((ptsQty < 1) || (ptsNewQty > ptsQty))
            needSubdrive = true;
        else
        {
            // Check distances.
            for (unsigned i = 0; i < ptsNewQty; i++)
            {
                const Vector3d & v = ptsNew_.ptr()[i];
                const Float d = d_close;
                Float minDist = -1.0;
                for (unsigned j = 0; j < ptsQty; j++)
                {
                    const Vector3d & a = pts_.ptr()[j];
                    const Float dot = v.DotProduct(a);
                    const Vector3d proj = a * dot;
                    const Vector3d diff = v - proj;
                    const Float dist = diff.Length();
                    if ((minDist < 0.0) || (dist < minDist))
                        minDist = dist;
                }
                if (minDist >= d)
                {
                    needSubdrive = true;
                    break;
                }
            }
        }
    }

    if ( needSubdrive )
    {
        pts_ = ptsNew_;
        flatten_pts( s );
    }

    return needSubdrive;
}

bool SubdivideSource::need_subdivide( const CubeSphere * s, const CubeQuadNode * f ) const
{
    const Float sz = f->size( s );
    const Vector3d n = f->normal( s );
    const unsigned ptsQty = ptsFlat_.size();
    const unsigned levelsQty = s->levelsUnit_.size();
    const unsigned lastLevelInd = levelsQty-1;
    for ( unsigned i=0; i<ptsQty; i++ )
    {
        const Vector3d & a = ptsFlat_.ptr()[i];
        for ( unsigned j=0; j<levelsQty; j++ )
        {
            const CubeSphere::Level & lvl = s->levelsUnit_.ptr()[j];
            //const bool inside = (j == lastLevelInd) || f->inside( s, a, n, lvl.dist );
            const bool inside = f->inside( s, a, n, lvl.dist );
            if ( inside )
            {
                if ( sz > lvl.sz )
                    return true;
            }
        }
    }
    return false;
}

void SubdivideSource::flatten_pts( const CubeSphere * s )
{
    s->flatten_pts( pts_, ptsFlat_ );
}





}







