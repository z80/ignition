
#ifndef __ONODE_H_
#define __ONODE_H_

#include "core/vector.h"
#include "core/math/math_defs.h"
#include "core/math/vector3.h"
#include "core/math/plane.h"
#include "core/math/face3.h"

class OTree2;

class ONode2
{
public:
	ONode2();
	~ONode2();
	ONode2( const ONode2 & inst );
	const ONode2 & operator=( const ONode2 & inst );

	bool hasChildren() const;
	bool subdivide();

	// Queries.
	// Intersection with a triangle.
	bool inside( const Face3 & face ) const;


	// Initialize vertices and planes.
	void init();

	OTree2 * tree;
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

	Vector<int> ptInds;
};


#endif



