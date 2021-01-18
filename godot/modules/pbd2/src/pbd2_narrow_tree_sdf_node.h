
#ifndef __PBD_NARROW_TREE_SDF_NODE_H_
#define __PBD_NARROW_TREE_SDF_NODE_H_

#include "data_types.h"
#include "se3.h"
#include "occupancy_types.h"

namespace Pbd
{

class NarrowTree;
class NarrowTreePtsNode;

class NarrowTreeSdfNode
{
public:
	NarrowTreeSdfNode();
	~NarrowTreeSdfNode();
	NarrowTreeSdfNode( const NarrowTreeSdfNode & inst );
	const NarrowTreeSdfNode & operator=( const NarrowTreeSdfNode & inst );

	bool hasChildren() const;
	bool subdivide();
	// Initialize vertices and planes.
	void init();


    bool collide_forward( const SE3 & se3_rel, const NarrowTreePtsNode * n, Vector<Vector3d> & pts, Vector<Vector3d> & depths ) const;

	NarrowTree * tree;
	int absIndex;
	int parentAbsIndex;
	int indexInParent;
	int level;

	// Child indices in Octtree list.
	int children[8];
        
    // Value is number of triangles inside.
	bool     on_or_below_surface;
	Float    size2; // Size over 2.
	Vector3d center;
        
    Cube cube_;

	// Distances to own mesh.
	Float d000, d100, d110, d010,
		  d001, d101, d111, d011;

	// Reset all to -1.0.
	void reset_distances();
	// Compute distances.
	void init_distances();
	// Probe sides and center of the node.
	void probe_distances( Vector3d * pts, Float * ds );
	Float probe_distance( const Vector3d & at ) const;
	Float probe_distance( const Vector3d & at, Vector3d & displacement ) const;
	// Check depth error provided by approximation against the real ones in the center
	// and on sides of the node.
	Float depth_error( Vector3d * pts, Float * ds );
	// Generate surface points for leaf nodes on the surface.
	void generate_surface_points();

	// Compute distance and gradient.
	bool distance( const Vector3d & r, Float & d, Vector3d & depth ) const;
	// These two are called internally.
	bool distance_recursive( const Vector3d & r, Float & d, Vector3d & ret ) const;
	Float distance_for_this_node( const Vector3d & r, Vector3d & disp ) const;

	// Determines if point is inside of a mesh or not.
	// By counting intersections.
	bool point_inside_mesh( const Vector3d & r ) const;

	// These two are in local coordinates.
	// No transforms should be applied.
	bool point_inside( const Vector3d & at ) const;
	bool intersects_ray( const Vector3d & r1, const Vector3d & r2 ) const;

	bool contains_surface() const;
	bool is_above() const;
	bool is_below() const;
	void compute_on_or_below_surface();
};

}


#endif



