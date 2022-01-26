
#include "subdivide_source_dist.h"
#include "cube_sphere.h"
#include "cube_quad_node.h"

namespace Ign
{

SubdivideSourceDist::SubdivideSourceDist()
    : SubdivideSource()
{
    _min_size  = 10.0;
    _min_angle = 0.1;
}

SubdivideSourceDist::~SubdivideSourceDist()
{
}


bool SubdivideSourceDist::need_subdivide( const CubeSphere * s, const Vector<Vector3d> & pts )
{
    const bool ret = SubdivideSource::need_subdivide( s, pts );
    return ret;
}

bool SubdivideSourceDist::need_subdivide( const CubeSphere * s, const CubeQuadNode * f ) const
{
    const Vector<CubeVertex> & verts = s->verts;
    const int ind0 = f->vertexInds[0];
    const int ind2 = f->vertexInds[2];
    const CubeVertex & cv0 = verts.ptr()[ind0];
    const CubeVertex & cv2 = verts.ptr()[ind2];
    const Vector3d dr = cv2.at - cv0.at;
    const Float sz = dr.Length();
    const bool smaller_than_min = (sz < _min_size);
    if ( smaller_than_min )
        return false;

    const Vector3d center = (cv0.at + cv2.at) * 0.5;

    const unsigned ptsQty = pts_.size();
    for ( unsigned i=0; i<ptsQty; i++ )
    {
        const Vector3d & a = pts_.ptr()[i];
        const Vector3d d = center - a;
        const Float dist = d.Length();
        if (dist < _min_size)
            return false;
        const Float angle = sz / dist;
        const bool divide = (angle > _min_angle);
        if ( divide )
            return true;
    }

    return false;
}







}







