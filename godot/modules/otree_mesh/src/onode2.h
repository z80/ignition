
#ifndef __ONODE_H_
#define __ONODE_H_

#include "core/vector.h"
#include "core/math/math_defs.h"
#include "core/math/vector3.h"
#include "core/math/plane.h"

class OTree2;

class ONode2
{
public:
	ONode2();
	~ONode2();
	ONode2( const ONode2 & inst );
	const ONode2 & operator=( const ONode2 & inst );

	ONode2 & parent();

	bool inside( const Vector3 & pt ) const;
	bool hasChildren() const;
	bool subdrive();

	void vertices( Vector3 * verts ) const;
	void planes( Plane * planes ) const;
	//bool intersectsRay( const Vector3 & start, const Vector3 & end ) const;

	// Queries.
	// Intersection with a triangle.

	OTree2 * tree;
	int absIndex;
	int parentAbsIndex;
	int indexInParent;
	int level;

	// Child indices in OTree dictionary.
	int children[8];

	real_t   value;
	real_t   size2; // Size over 2.
	Vector3 center;

	Vector<int> ptInds;
};


#endif



