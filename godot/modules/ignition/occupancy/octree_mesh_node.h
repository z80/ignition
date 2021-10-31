
#ifndef __OCTREE_MESH_NODE_H_
#define __OCTREE_MESH_NODE_H_

#include "core/vector.h"
#include "core/math/math_defs.h"
#include "core/math/vector3.h"
#include "core/math/plane.h"
#include "core/math/face3.h"

namespace Ign
{

class OctreeMesh;

class OctreeMeshNode
{
public:
	OctreeMeshNode();
	~OctreeMeshNode();
	OctreeMeshNode( const OctreeMeshNode & inst );
	const OctreeMeshNode & operator=( const OctreeMeshNode & inst );

	bool hasChildren() const;
	bool subdivide();

	// Queries.
	// Intersection with a triangle.
	bool inside( const Face3 & face ) const;
	// Intersects infinite ray.
	bool intersects_ray( const Vector3 origin, const Vector3 dir ) const;
	bool intersects_ray_face( const Vector3 origin, const Vector3 dir, int & face_ind, real_t & dist ) const;
	// Intersects line segment.
	bool intersects_segment( const Vector3 start, const Vector3 end ) const;
	bool intersects_segment_face( const Vector3 start, const Vector3 end, int & face_ind, real_t & dist ) const;

	// Initialize vertices and planes.
	void init();

	OctreeMesh * tree;
	int absIndex;
	int parentAbsIndex;
	int indexInParent;
	int level;

	// Child indices in OTree dictionary.
	int children[8];

	real_t  size2; // Size over 2.
	Vector3 center;

	// Auxilary numbers. Hold those only to not recompute all the time.
	Plane   planes_[6];
	AABB    aabb_;

	Vector<int> ptInds;
};

}


#endif



