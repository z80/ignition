#include "register_types.h"

#include "core/class_db.h"

//#include "occupancy_grid.h"
#include "octree_mesh_gd.h"
#include "broad_tree_gd.h"

#include "ref_frame_node.h"
#include "ref_frame_auto_node.h"
#include "se3_ref.h"
#include "distance_scaler_ref.h"

// Sphere related classes.
#include "height_source_ref.h"
#include "height_source_test_ref.h"
#include "height_source_gd_ref.h"
#include "cube_sphere_node.h"

// Celestial motion.
#include "celestial_motion_ref.h"
#include "celestial_rotation_ref.h"

void register_ignition_types()
{
	//ClassDB::register_class<OccupancyGrid>();
	ClassDB::register_class<Ign::OctreeMeshGd>();
	ClassDB::register_class<Ign::BroadTreeGd>();

	ClassDB::register_class<Ign::RefFrameNode>();
	ClassDB::register_class<Ign::RefFrameAutoNode>();
	ClassDB::register_class<Ign::Se3Ref>();
	ClassDB::register_class<Ign::DistanceScalerRef>();

	ClassDB::register_class<Ign::HeightSourceRef>();
	ClassDB::register_class<Ign::HeightSourceTestRef>();
	ClassDB::register_class<Ign::HeightSourceGdRef>();

	ClassDB::register_class<Ign::SubdivideSourceRef>();
	ClassDB::register_class<Ign::CubeSphereNode>();

	ClassDB::register_class<Ign::CelestialMotionRef>();
	ClassDB::register_class<Ign::CelestialRotationRef>();
}

void unregister_ignition_types()
{
}
