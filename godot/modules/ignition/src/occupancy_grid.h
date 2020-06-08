
#ifndef __OCCUPANCY_GRID_H_
#define __OCCUPANCY_GRID_H_

#include "core/vector.h"
#include "core/math/face3.h"
#include "scene/main/node.h"
#include "scene/3d/mesh_instance.h"
#include "grid_node.h"


/**
	This data structure is for determining empty/filled space only.
	Nonempty leaf node means space is filled. Else means it's empty.
**/

class OccupancyGrid
{
	GDCLASS(OccupancyGrid, Node);

protected:
	static void _bind_methods();

public:
	OccupancyGrid();
    ~OccupancyGrid();

	OccupancyGrid( const OccupancyGrid & inst );
    const OccupancyGrid & operator=( const OccupancyGrid & inst );

	void setNodeSize( real_t sz = 0.1 );
	real_t nodeSize() const;
	void clear();
    void append( const Transform & t, const Ref<Mesh> mesh );
	void subdivide();
    
    bool parent( const GridNode & node, GridNode * & parent );

    int  insertNode( GridNode & node );
    void updateNode( const GridNode & node );

    Vector<GridNode> nodes_;
    Vector<Face3>  faces_;

    // Maximum subdivision level.
	real_t node_sz_;
    int    max_depth_;
};







#endif





