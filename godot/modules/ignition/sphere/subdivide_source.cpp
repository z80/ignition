
#include "subdivide_source.h"


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
    levels_.Clear();
}

void SubdivideSource::add_level( Float sz, Float dist )
{
    Level lvl;
    lvl.sz   = sz;
    lvl.dist = dist;
    levels_.Push( lvl );
}

bool SubdivideSource::need_subdivide( const Cubesphere * s, Vector<SubdividePoint> & pts )
{
    ptsNew_ = pts;
    const unsigned ptsNewQty = ptsNew_.Size();
    for ( unsigned i=0; i<ptsNewQty; i++ )
    {
        Vector3d & v = ptsNew_[i].at;
        v.Normalize();
    }

    if ( levels_.Empty() )
    {
        pts_ = ptsNew_;
        flattenPts( s );
        return true;
    }

    // Sort and normalize all levels.
    sortLevels( s );
    const Level lvl_close = levelsUnit_[0];
    const Float d_close = lvl_close.dist * 0.5;
    const Level lvl_far = levelsUnit_[levelsUnit_.Size() - 1];
    const Float d_far   = lvl_far.dist * 0.5;


    // Check all distances. And resubdrive if shifted half the finest distance.
    const unsigned ptsQty = pts_.Size();
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
				const Vector3d & v = ptsNew_[i].at;
                const bool isClose = ptsNew_[i].close;
                const Float d = isClose ? d_close : d_far;
				Float minDist = -1.0;
				for (unsigned j = 0; j < ptsQty; j++)
				{
					const Vector3d & a = pts_[j].at;
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
        flattenPts( s );
    }

    return needSubdrive;
}

bool SubdivideSource::need_subdivide( const Cubesphere * s, const CubeFace * f ) const
{
    const Float sz = f->size( s );
    const Vector3d n = f->normal( s );
    const unsigned ptsQty = ptsFlat_.Size();
    const unsigned levelsQty = levelsUnit_.Size();
    const unsigned lastLevelInd = levelsQty-1;
    for ( unsigned i=0; i<ptsQty; i++ )
    {
        const Vector3d & a = ptsFlat_[i].at;
        const bool isClose = ptsFlat_[i].close;
        const unsigned startInd = (isClose) ? 0 : (levelsQty-1);
        for ( unsigned j=startInd; j<levelsQty; j++ )
        {
            const Level & lvl = levelsUnit_[j];
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
    s->flattenPts( pts_, ptsFlat_ );
}

void SubdivideSource::sort_levels( const CubeSphere * s )
{
    levelsUnit_ = levels_;

    // Normalize all distances.
    const Float R = s->R();
    const unsigned qty = levelsUnit_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Level & a = levelsUnit_[i];
        a.sz   /= R;
        a.dist /= R;
    }

    // Sort levels in distance accending order.
    for ( unsigned i=0; i<(qty-1); i++ )
    {
        Level & a = levelsUnit_[i];
        for ( unsigned j=(i+1); j<qty; j++ )
        {
            Level & b = levelsUnit_[j];
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







