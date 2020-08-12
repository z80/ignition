
#ifndef __OCCUPANCY_GRID_H_
#define __OCCUPANCY_GRID_H_

#include "core/reference.h"
#include "core/vector.h"
#include "core/math/face3.h"
#include "scene/main/node.h"
#include "scene/3d/mesh_instance.h"
#include "grid_node.h"


/**
	This data structure is for determining empty/filled space only.
	Nonempty leaf node means space is filled. Else means it's empty.
**/

class OccupancyGrid: public Reference
{
	GDCLASS(OccupancyGrid, Reference);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	OccupancyGrid();
    ~OccupancyGrid();

    // Debug methods.
    int accum;
    void d_init( int v );
    void d_add( int v );
    int  d_result();

	//OccupancyGrid( const OccupancyGrid & inst );

	// Manipulating filling up the occupancy grid.
	void set_node_size( real_t sz = 0.1 );
	real_t node_size() const;
	void clear();
    void append( const Transform & t, const Ref<Mesh> mesh );
	void subdivide();

	// Check if certain point is occupied.
	bool occupied( const Vector3 & at ) const;
	// Internally called for recursion.
	bool point_inside( const GridNode & n, const Vector3 & at ) const;
	// Point ajacent.
	bool point_ajacent( const Vector3 & at ) const;
	// Intersects.
	bool intersects( const Ref<OccupancyGrid> g ) const;
	// Internally called for recursion.
	bool node_intersects( const GridNode & n, const OccupancyGrid & g ) const;
	// Touches.
	bool touches( const Ref<OccupancyGrid> g ) const;
	Vector3 touch_point( const Ref<OccupancyGrid> g ) const;
	// Intersects with infinite ray.
	bool intersects_ray( const Vector3 p_from, const Vector3 p_to ) const;

	// Moving entire tree to a different location.
	void set_position( const Vector3 & at );
	Vector3 get_position() const;
	// Internally used for recursive calls.
	void set_node_position( GridNode & n, const Vector3 from, const Vector3 to );

	// For visualization.
	PoolVector<Vector3> lines();
    
    bool parent( const GridNode & node, GridNode * & parent );

    int  insertNode( GridNode & node );
    void updateNode( const GridNode & node );

    Vector<GridNode> nodes_;
    Vector<Face3>    faces_;

    // Maximum subdivision level.
	real_t node_sz_;
    int    max_depth_;
};







#endif





