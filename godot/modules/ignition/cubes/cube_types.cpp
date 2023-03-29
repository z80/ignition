
#include "cube_types.h"


namespace Ign
{

VectorInt operator+( const VectorInt & a, const VectorInt & b )
{
	const VectorInt ret = VectorInt( a.x+b.x, a.y+b.y, a.z+b.z );
	return ret;
}

bool operator<( const NodeFace & a, const NodeFace & b )
{
	const bool ret = (a.cell < b.cell);
	return ret;
}

static const Float EPS = 1.0e-8;

bool ray_intersects_triangle( const Vector3d &p_from, const Vector3d &p_dir, const Vector3d &p_v0, const Vector3d &p_v1, const Vector3d &p_v2, Vector3d *r_res )
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
	else
	{ // This means that there is a line intersection but not a ray intersection.
		return false;
	}
}

bool segment_intersects_triangle(const Vector3d &p_from, const Vector3d &p_to, const Vector3d &p_v0, const Vector3d &p_v1, const Vector3d &p_v2, Vector3d *r_res )
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
	else
	{ // This means that there is a line intersection but not a ray intersection.
		return false;
	}
}






bool NodeFace::intersects_ray(const Vector3d & p_from, const Vector3d &p_dir, Vector3d *p_intersection ) const
{
	const bool ret = ray_intersects_triangle( p_from, p_dir, vertices[0], vertices[1], vertices[2], p_intersection );
	return ret;
}

bool NodeFace::intersects_segment(const Vector3d & p_from, const Vector3d &p_to, Vector3d *p_intersection ) const
{
	const bool ret = segment_intersects_triangle( p_from, p_to, vertices[0], vertices[1], vertices[2], p_intersection );
	return ret;
}

bool Aabb::has_point(const Vector3d &p_point) const
{
	if (p_point.x_ < position.x_)
	{
		return false;
	}
	if (p_point.y_ < position.y_)
	{
		return false;
	}
	if (p_point.z_ < position.z_)
	{
		return false;
	}
	if (p_point.x_ > position.x_ + size.x_)
	{
		return false;
	}
	if (p_point.y_ > position.y_ + size.y_)
	{
		return false;
	}
	if (p_point.z_ > position.z_ + size.z_)
	{
		return false;
	}

	return true;
}


bool Aabb::intersects_segment( const Vector3d & p_from, const Vector3d & p_to, Vector3d * r_clip, Vector3d * r_normal ) const
{
	Float min = 0, max = 1;
	int axis = 0;
	Float sign = 0;

	for (int i = 0; i<3; i++)
	{
		Float seg_from = p_from[i];
		Float seg_to = p_to[i];
		Float box_begin = position[i];
		Float box_end = box_begin + size[i];
		Float cmin, cmax;
		Float csign;

		if (seg_from < seg_to)
		{
			if (seg_from > box_end || seg_to < box_begin)
			{
				return false;
			}
			Float length = seg_to - seg_from;
			cmin = (seg_from < box_begin) ? ((box_begin - seg_from) / length) : 0;
			cmax = (seg_to > box_end) ? ((box_end - seg_from) / length) : 1;
			csign = -1.0;

		}
		else
		{
			if (seg_to > box_end || seg_from < box_begin)
			{
				return false;
			}
			Float length = seg_to - seg_from;
			cmin = (seg_from > box_end) ? (box_end - seg_from) / length : 0;
			cmax = (seg_to < box_begin) ? (box_begin - seg_from) / length : 1;
			csign = 1.0;
		}

		if (cmin > min)
		{
			min = cmin;
			axis = i;
			sign = csign;
		}
		if (cmax < max)
		{
			max = cmax;
		}
		if (max < min)
		{
			return false;
		}
	}

	Vector3d rel = p_to - p_from;

	if (r_normal)
	{
		//Vector3 normal;
		//normal[axis] = sign;
		(*r_normal)[axis] = sign;
	}

	if (r_clip)
	{
		*r_clip = p_from + rel * min;
	}

	return true;
}

bool Aabb::intersects_ray( const Vector3d & p_from, const Vector3d & p_dir, Vector3d * r_clip, Vector3d * r_normal ) const
{
	Vector3d c1, c2;
	Vector3d end = position + size;
	Float near = -1e20;
	Float far = 1e20;
	int axis = 0;

	for ( int i = 0; i<3; i++ )
	{
		if ( p_dir[i] == 0.0)
		{
			if ( (p_from[i] < position[i]) || (p_from[i] > end[i]) )
			{
				return false;
			}
		} else { // ray not parallel to planes in this direction
			c1[i] = (position[i] - p_from[i]) / p_dir[i];
			c2[i] = (end[i] - p_from[i]) / p_dir[i];

			if (c1[i] > c2[i]) {
				SWAP(c1, c2);
			}
			if (c1[i] > near)
			{
				near = c1[i];
				axis = i;
			}
			if (c2[i] < far)
			{
				far = c2[i];
			}
			if ((near > far) || (far < 0))
			{
				return false;
			}
		}
	}

	if (r_clip)
	{
		*r_clip = c1;
	}
	if (r_normal)
	{
		//*r_normal = Vector3d();
		(*r_normal)[axis] = ( p_dir[axis] > 0.0 ) ? -1.0 : 1.0;
	}

	return true;
}



}
