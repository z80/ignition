#include "register_types.h"

#include "core/class_db.h"
#include "occupancy_grid.h"
//#include "ref_frame.h"
//#include "ref_frame_tree.h"
#include "ref_frame_node.h"
#include "ref_frame_auto_node.h"
#include "distance_scaler_ref.h"

// Sphere related classes.
#include "height_source_ref.h"
#include "height_source_test_ref.h"
#include "cube_sphere_node.h"

void register_ignition_types()
{
	ClassDB::register_class<OccupancyGrid>();
	//ClassDB::register_class<RefFrame>();
	//ClassDB::register_class<RefFrameTree>();
	ClassDB::register_class<RefFrameNode>();

	ClassDB::register_class<Ign::HeightSourceRef>();
	ClassDB::register_class<Ign::HeightSourceTestRef>();
	ClassDB::register_class<Ign::CubeSphereNode>();
}

void unregister_ignition_types()
{
}
