#include "register_types.h"

#include "core/class_db.h"
#include "occupancy_grid.h"
//#include "ref_frame.h"
//#include "ref_frame_tree.h"
#include "ref_frame_node.h"

void register_ignition_types()
{
	ClassDB::register_class<OccupancyGrid>();
	//ClassDB::register_class<RefFrame>();
	//ClassDB::register_class<RefFrameTree>();
	ClassDB::register_class<RefFrameNode>();
}

void unregister_ignition_types()
{
}
