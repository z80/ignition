
#include "pbd2_joint_hinge_node.h"

namespace Pbd
{

PbdJointHingeNode::PbdJointHingeNode()
    : PbdJoint()
{
    joint = &joint_hinge;
}

PbdJointHingeNode::~PbdJointHingeNode()
{

}

void PbdJointHingeNode::set_spatial_gap( real_t r )
{
    joint_hinge.spatial_gap = r;
}

real_t PbdJointHingeNode::get_spatial_gap() const
{
    return joint_hinge.spatial_gap;
}

void PbdJointHingeNode::set_angular_gap( real_t r )
{
    joint_hinge.angular_gap = r;
}

real_t PbdJointHingeNode::get_angular_gap() const
{
    return joint_hinge.angular_gap;
}

void PbdJointHingeNode::set_target_position( real_t r )
{
    joint_hinge.target_position = r;
}

real_t PbdJointHingeNode::get_target_position() const
{
    return joint_hinge.target_position;
}

void PbdJointHingeNode::set_target_velocity( real_t v )
{
    joint_hinge.target_velocity = v;
}

real_t PbdJointHingeNode::get_target_velocity() const
{
    return joint_hinge.target_velocity;
}

void PbdJointHingeNode::set_motor_gap( real_t r )
{
    joint_hinge.motor_gap = r;
}

real_t PbdJointHingeNode::get_motor_gap() const
{
    return joint_hinge.motor_gap;
}

void PbdJointHingeNode::set_position_control( bool en )
{
    joint_hinge.position_control = en;
}

bool PbdJointHingeNode::get_position_control() const
{
    return joint_hinge.position_control;
}

void PbdJointHingeNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_spatial_gap", "d" ), &PbdRigidBodyNode::set_spatial_gap );
    ClassDB::bind_method( D_METHOD( "get_spatial_gap" ),      &PbdRigidBodyNode::get_spatial_gap, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_angular_gap", "d" ), &PbdRigidBodyNode::set_angular_gap );
    ClassDB::bind_method( D_METHOD( "get_angular_gap" ),      &PbdRigidBodyNode::get_angular_gap, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_target_position", "r" ), &PbdRigidBodyNode::set_target_position );
    ClassDB::bind_method( D_METHOD( "get_target_position" ),      &PbdRigidBodyNode::get_target_position, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_target_velocity", "r" ), &PbdRigidBodyNode::set_target_velocity );
    ClassDB::bind_method( D_METHOD( "get_target_velocity" ),      &PbdRigidBodyNode::get_target_velocity, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_motor_gap", "d" ), &PbdRigidBodyNode::set_motor_gap );
    ClassDB::bind_method( D_METHOD( "get_motor_gap" ),      &PbdRigidBodyNode::get_motor_gap, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_position_control", "d" ), &PbdRigidBodyNode::set_position_control );
    ClassDB::bind_method( D_METHOD( "get_position_control" ),      &PbdRigidBodyNode::get_position_control, Variant::BOOL );


    ADD_PROPERTY( PropertyInfo( Variant::REAL, "spatial_gap" ), &PbdRigidBodyNode::set_spatial_gap, &PbdRigidBodyNode::get_spatial_gap );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "angular_gap" ), &PbdRigidBodyNode::set_angular_gap, &PbdRigidBodyNode::get_angular_gap );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "target_position" ), &PbdRigidBodyNode::set_target_position, &PbdRigidBodyNode::get_target_position );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "target_velocity" ), &PbdRigidBodyNode::set_target_velocity, &PbdRigidBodyNode::get_target_velocity );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "motor_gap" ), &PbdRigidBodyNode::set_motor_gap, &PbdRigidBodyNode::get_motor_gap );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "position_control" ), &PbdRigidBodyNode::set_position_control, &PbdRigidBodyNode::get_position_control );
}



}









