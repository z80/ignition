#include "register_types.h"

#include "core/class_db.h"

//#include "occupancy_grid.h"
#include "octree_mesh_gd.h"
#include "broad_tree_gd.h"

#include "ref_frame_node.h"
#include "ref_frame_root.h"
#include "ref_frame_motion_node.h"
#include "ref_frame_rotation_node.h"
#include "ref_frame_non_inertial_node.h"
#include "ref_frame_body_node.h"
#include "ref_frame_super_body_node.h"
//#include "ref_frame_auto_node.h"
#include "se3_ref.h"
#include "distance_scaler_ref.h"

// Sphere related classes.
#include "height_source_ref.h"
#include "height_source_test_ref.h"
#include "height_source_gd_ref.h"
#include "cube_sphere_node.h"
#include "subdivide_source_ref.h"
#include "subdivide_source_dist_ref.h"

// Celestial motion.
#include "celestial_motion_ref.h"
#include "celestial_rotation_ref.h"


// Volume.
#include "marching_volume_object_gd.h"
#include "volume_source_gd.h"
#include "volume_source_script_gd.h"
#include "volume_source_tree_gd.h"
//#include "material_source_gd.h"
//#include "material_source_script_gd.h"
#include "marching_cubes_gd.h"
#include "marching_cubes_dual_gd.h"
#include "marching_cubes_dual_node_gd.h"

#include "marching_cubes_rebuild_strategy_gd.h"
#include "volume_node_size_strategy_gd.h"

#include "ign_random_gd.h"

#include "iir2.h"

void register_ignition_types()
{
	//ClassDB::register_class<OccupancyGrid>();
	ClassDB::register_class<Ign::OctreeMeshGd>();
	ClassDB::register_class<Ign::BroadTreeGd>();

	ClassDB::register_class<Ign::RefFrameNode>();
	ClassDB::register_class<Ign::RefFrameRoot>();
	ClassDB::register_class<Ign::RefFrameMotionNode>();
	ClassDB::register_class<Ign::RefFrameRotationNode>();
	ClassDB::register_class<Ign::RefFrameNonInertialNode>();
	ClassDB::register_class<Ign::RefFrameBodyNode>();
	ClassDB::register_class<Ign::RefFrameSuperBodyNode>();
	//ClassDB::register_class<Ign::RefFrameAutoNode>();
	ClassDB::register_class<Ign::Se3Ref>();
	ClassDB::register_class<Ign::DistanceScalerBaseRef>();
	ClassDB::register_class<Ign::DistanceScalerRef>();

	ClassDB::register_class<Ign::HeightSourceRef>();
	ClassDB::register_class<Ign::HeightSourceTestRef>();
	ClassDB::register_class<Ign::HeightSourceGdRef>();

	ClassDB::register_class<Ign::SubdivideSourceRef>();
	ClassDB::register_class<Ign::SubdivideSourceDistRef>();
	ClassDB::register_class<Ign::CubeSphereNode>();

	ClassDB::register_class<Ign::CelestialMotionRef>();
	ClassDB::register_class<Ign::CelestialRotationRef>();

	ClassDB::register_class<Ign::MarchingVolumeObjectGd>();
	ClassDB::register_class<Ign::VolumeSourceGd>();
	ClassDB::register_class<Ign::VolumeSourceScriptGd>();
	ClassDB::register_class<Ign::VolumeSourceTreeGd>();
	
	ClassDB::register_class<Ign::MarchingCubesGd>();
	ClassDB::register_class<Ign::MarchingCubesDualGd>();
	ClassDB::register_class<Ign::MarchingCubesDualNodeGd>();

	ClassDB::register_class<Ign::MarchingCubesRebuildStrategyGd>();
	ClassDB::register_class<Ign::VolumeNodeSizeStrategyGd>();

	ClassDB::register_class<Ign::IgnRandomGd>();


	ClassDB::register_class<Iir2::Iir2sRef>();
}

void unregister_ignition_types()
{
}
