
#include "vol_data_types.h"

namespace Ign
{

VolVectorInt operator+( const VolVectorInt & a, const VolVectorInt & b )
{
	const VolVectorInt ret = VolVectorInt( a.x+b.x, a.y+b.y, a.z+b.z );
	return ret;
}

VolVectorInt operator+( const VolVectorInt & a, Integer b )
{
	const VolVectorInt ret = VolVectorInt( a.x+b, a.y+b, a.z+b );
	return ret;
}

bool operator<( const VolFace & a, const VolFace & b )
{
	const bool ret = (a.node < b.node);
	return ret;
}

static const Float EPS = 1.0e-8;

static bool ray_intersects_triangle( const Vector3d &p_from, const Vector3d &p_dir, const Vector3d &p_v0, const Vector3d &p_v1, const Vector3d &p_v2, Vector3d *r_res )
{
	const Vector3d e1 = p_v1 - p_v0;
	const Vector3d e2 = p_v2 - p_v0;
	const Vector3d h = p_dir.CrossProduct(e2);
	const Float a = e1.DotProduct( h );
	if ( std::abs(a) < EPS )
	{ // Parallel test.
		return false;
	}

	const Float f = 1.0 / a;

	const Vector3d s = p_from - p_v0;
	const Float u = f * s.DotProduct(h);

	if ( (u < 0.0) || (u > 1.0) )
	{
		return false;
	}

	const Vector3d q = s.CrossProduct( e1 );

	const Float v = f * p_dir.DotProduct( q );

	if ( (v < 0.0) || (u + v > 1.0) )
	{
		return false;
	}

	// At this stage we can compute t to find out where
	// the intersection point is on the line.
	const Float t = f * e2.DotProduct( q );

	if ( t > 0.00001 )
	{ // ray intersection
		if ( r_res )
		{
			*r_res = p_from + p_dir * t;
		}
		return true;
	}
	// This means that there is a line intersection but not a ray intersection.
	return false;
}

static bool segment_intersects_triangle(const Vector3d &p_from, const Vector3d &p_to, const Vector3d &p_v0, const Vector3d &p_v1, const Vector3d &p_v2, Vector3d *r_res )
{
	const Vector3d rel = p_to - p_from;
	const Vector3d e1 = p_v1 - p_v0;
	const Vector3d e2 = p_v2 - p_v0;
	const Vector3d h = rel.CrossProduct( e2 );
	const Float a = e1.DotProduct( h );
	if ( std::abs( a ) < EPS )
	{ // Parallel test.
		return false;
	}

	const Float f = 1.0 / a;

	const Vector3d s = p_from - p_v0;
	const Float u = f * s.DotProduct(h);

	if ( (u < 0.0) || (u > 1.0) )
	{
		return false;
	}

	const Vector3d q = s.CrossProduct(e1);

	const Float v = f * rel.DotProduct( q );

	if ( (v < 0.0) || (u + v > 1.0) )
	{
		return false;
	}

	// At this stage we can compute t to find out where
	// the intersection point is on the line.
	const Float t = f * e2.DotProduct( q );

	if ( (t > EPS) && (t <= 1.0) )
	{ // Ray intersection.
		if (r_res)
		{
			*r_res = p_from + rel * t;
		}
		return true;
	}
	
	// This means that there is a line intersection but not a ray intersection.
	return false;
}






bool VolFace::intersects_ray(const Vector3d & p_from, const Vector3d &p_dir, Vector3d *p_intersection ) const
{
	const bool ret = ray_intersects_triangle( p_from, p_dir, vertices[0], vertices[1], vertices[2], p_intersection );
	return ret;
}

bool VolFace::intersects_segment(const Vector3d & p_from, const Vector3d &p_to, Vector3d *p_intersection ) const
{
	const bool ret = segment_intersects_triangle( p_from, p_to, vertices[0], vertices[1], vertices[2], p_intersection );
	return ret;
}


}


