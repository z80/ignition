
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

void SubdivideSource::clear_levels()
{
    levels_.clear();
}

void SubdivideSource::add_level( Float sz, Float dist )
{
    Level lvl;
    lvl.sz   = sz;
    lvl.dist = dist;
    levels_.push_back( lvl );
}

bool SubdivideSource::need_subdivide( const CubeSphere * s, Vector<SubdividePoint> & pts )
{
    ptsNew_ = pts;
    const unsigned ptsNewQty = ptsNew_.size();
    for ( unsigned i=0; i<ptsNewQty; i++ )
    {
        Vector3d & v = ptsNew_.ptrw()[i].at;
        v.Normalize();
    }

    if ( levels_.empty() )
    {
        pts_ = ptsNew_;
        flatten_pts( s );
        return true;
    }

    // Sort and normalize all levels.
    sort_levels( s );
    const Level lvl_close = levelsUnit_.ptr()[0];
    const Float d_close = lvl_close.dist * 0.5;
    const Level lvl_far = levelsUnit_.ptr()[levelsUnit_.size() - 1];
    const Float d_far   = lvl_far.dist * 0.5;


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
				const SubdividePoint & sp = ptsNew_.ptr()[i];
				const Vector3d & v = sp.at;
                const bool isClose = sp.close;
                const Float d = isClose ? d_close : d_far;
				Float minDist = -1.0;
				for (unsigned j = 0; j < ptsQty; j++)
				{
					const Vector3d & a = pts_.ptr()[j].at;
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
    const unsigned levelsQty = levelsUnit_.size();
    const unsigned lastLevelInd = levelsQty-1;
    for ( unsigned i=0; i<ptsQty; i++ )
    {
        const Vector3d & a = ptsFlat_.ptr()[i].at;
        const bool isClose = ptsFlat_.ptr()[i].close;
        const unsigned startInd = (isClose) ? 0 : (levelsQty-1);
        for ( unsigned j=startInd; j<levelsQty; j++ )
        {
            const Level & lvl = levelsUnit_.ptr()[j];
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

void SubdivideSource::sort_levels( const CubeSphere * s )
{
    levelsUnit_ = levels_;

    // Normalize all distances.
    const Float R = s->r();
    const unsigned qty = levelsUnit_.size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Level & a = levelsUnit_.ptrw()[i];
        a.sz   /= R;
        a.dist /= R;
    }

    // Sort levels in distance accending order.
    for ( unsigned i=0; i<(qty-1); i++ )
    {
        Level & a = levelsUnit_.ptrw()[i];
        for ( unsigned j=(i+1); j<qty; j++ )
        {
            Level & b = levelsUnit_.ptrw()[j];
            if ( a.dist > b.dist )
            {
                const Level c = a;
                a = b;
                b = c;
            }
        }
    }

}





}







