
#ifndef __PBD_NARROW_TREE_NODE_H_
#define __PBD_NARROW_TREE_NODE_H_

#include "data_types.h"
#include "se3.h"
#include "occupancy_types.h"


namespace Pbd
{

class NarrowTree;

class NarrowTreeNode
{
public:
	NarrowTreeNode();
	~NarrowTreeNode();
	NarrowTreeNode( const NarrowTreeNode & inst );
	const NarrowTreeNode & operator=( const NarrowTreeNode & inst );

    void apply( const SE3 & se3 );

	bool hasChildren() const;
	bool subdivide();

	// Queries.
	// Intersection with a triangle. It is for initial fill up.
	bool inside( const Face & face ) const;
    // This is for recursive collision detection.
    // Will need to extend it with "optimal" box later.
	bool inside( NarrowTreeNode & n );


	// Initialize vertices and planes.
	void init();


    bool collide_forward( NarrowTreeNode & n, Vector<Vector3d> & pts, Vector<Vector3d> & depths );
    bool collide_backward( NarrowTreeNode & this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths );
    bool collide_faces( NarrowTreeNode & this_node, Vector<Vector3d> & pts, Vector<Vector3d> & depths );

    // Compute "se3_optimized_" and "cube_optimized_".
    bool compute_cube_optimized();

	NarrowTree * tree;
	int absIndex;
	int parentAbsIndex;
	int indexInParent;
	int level;

	// Child indices in Octtree list.
	int children[8];
        
    // Value is number of triangles inside.
	int      value;
	Float    size2; // Size over 2.
	Vector3d center;
        
    Cube cube_;
    SE3  se3_optimized_;
    Cube cube_optimized_;

	// Distances to own mesh.
	Float d000, d100, d110, d010,
		  d001, d101, d111, d011;

	// Reset all to -1.0.
	void reset_distances();
	// Compute distances.
	void init_distances();
	// Compute distance and gradient.
	Float distance( const Vector3d & r, Vector3d & d ) const;
	// These two are called internally.
	Float distance_recursive( const Vector3d & r, Vector3d & ret ) const;
	Float distance_for_this_node( const Vector3d & r, Vector3d & disp ) const;

	// Determines if point is inside of a mesh or not.
	// By counting intersections.
	bool point_inside_mesh( const Vector3d & r ) const;

	// These two are in local coordinates.
	// No transforms should be applied.
	bool point_inside( const Vector3d & at ) const;
	bool intersects_ray( const Vector3d & r1, const Vector3d & r2 ) const;


	Vector<int> ptInds;
};

}


#endif



