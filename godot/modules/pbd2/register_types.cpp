
#include "register_types.h"

#include "core/engine.h"
#include "core/class_db.h"

#include "pbd2_simulation_node.h"
#include "pbd2_rigid_body_node.h"
#include "pbd2_joint_node.h"
#include "pbd2_joint_hinge_node.h"
#include "pbd2_joint_ball_node.h"
#include "pbd2_contact_point_node.h"

void register_pbd2_types()
{
    ClassDB::register_class<Pbd::PbdSimulationNode>();
    ClassDB::register_class<Pbd::PbdRigidBodyNode>();
    ClassDB::register_class<Pbd::PbdJointNode>();
    ClassDB::register_class<Pbd::PbdJointHingeNode>();
    ClassDB::register_class<Pbd::PbdJointBallNode>();
	ClassDB::register_class<Pbd::PbdContactPointNode>();
}

void unregister_pbd2_types()
{
}




