
#ifndef __GRID_NODE_H_
#define __GRID_NODE_H_

#include "core/vector.h"
#include "core/math/math_defs.h"
#include "core/math/vector3.h"
#include "core/math/plane.h"
#include "core/math/face3.h"

class OccupancyGrid;

class GridNode
{
public:
	GridNode();
	~GridNode();
	GridNode( const GridNode & inst );
	const GridNode & operator=( const GridNode & inst );

	bool hasChildren() const;
	bool subdivide();

	// Queries.
	// Intersection with a triangle.
	bool inside( const Face3 & face ) const;
	bool inside( const Vector3 & pt ) const;

	// Initialize vertices and planes.
	void init();

	OccupancyGrid * tree;
	int absIndex;
	int parentAbsIndex;
	int indexInParent;
	int level;

	// Child indices in OTree dictionary.
	int children[8];

	int      value;
	real_t   size2; // Size over 2.
	Vector3 center;

	// Auxilary numbers. Hold those only to not recompute all the time.
	Vector3 verts_[8];
	Plane   planes_[6];
	AABB    aabb_;

	Vector<int> ptInds;
};


#endif



