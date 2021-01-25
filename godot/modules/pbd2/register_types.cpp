
#include "register_types.h"

#include "core/engine.h"
#include "core/class_db.h"

// Physics simulation
#include "pbd2_simulation_node.h"
// Rigid body
#include "pbd2_rigid_body_node.h"

// Joints.
#include "pbd2_joint_node.h"
#include "pbd2_joint_hinge_node.h"
#include "pbd2_joint_ball_node.h"
// Contact points against y=0 plane.
#include "pbd2_contact_point_node.h"

// Collision objects.
#include "pbd2_collision_object_node.h"
#include "pbd2_collision_sphere_node.h"
#include "pbd2_collision_plane_node.h"
#include "pbd2_collision_box_node.h"

#include "pbd2_narrow_tree_node.h"
#include "occupancy_tests_ref.h"

void register_pbd2_types()
{
    ClassDB::register_class<Pbd::PbdSimulationNode>();
    ClassDB::register_class<Pbd::PbdRigidBodyNode>();
    ClassDB::register_class<Pbd::PbdJointNode>();
    ClassDB::register_class<Pbd::PbdJointHingeNode>();
    ClassDB::register_class<Pbd::PbdJointBallNode>();
    ClassDB::register_class<Pbd::PbdContactPointNode>();
    ClassDB::register_class<Pbd::PbdCollisionObjectNode>();
    ClassDB::register_class<Pbd::PbdCollisionSphereNode>();
    ClassDB::register_class<Pbd::PbdCollisionPlaneNode>();
    ClassDB::register_class<Pbd::PbdCollisionBoxNode>();

    ClassDB::register_class<Pbd::PbdNarrowTreeNode>();
    ClassDB::register_class<Pbd::OccupancyTestsRef>();
}

void unregister_pbd2_types()
{
}




