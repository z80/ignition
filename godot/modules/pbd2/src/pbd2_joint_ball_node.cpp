
#include "pbd2_joint_ball_node.h"


namespace Pbd
{

PbdJointBallNode::PbdJointBallNode()
    : PbdJointNode()
{
    joint = &joint_ball;
}

PbdJointBallNode::~PbdJointBallNode()
{
}

void PbdJointBallNode::set_spatial_gap( real_t r )
{
    joint_ball.spatial_gap = r;
}

real_t PbdJointBallNode::get_spatial_gap() const
{
    return joint_ball.spatial_gap;
}

void PbdJointBallNode::set_angular_gap( real_t a )
{
    joint_ball.angular_gap = a;
}

real_t PbdJointBallNode::get_angular_gap() const
{
    return joint_ball.angular_gap;
}

void PbdJointBallNode::set_motor_gap( real_t r )
{
    joint_ball.motor_gap = r;
}

real_t PbdJointBallNode::get_motor_gap() const
{
    return joint_ball.motor_gap;
}

void PbdJointBallNode::set_target_position( const Quat & q )
{
    const Quaterniond Q( q.w, q.x, q.y, q.z );
    joint_ball.target_q = Q;
}

Quat PbdJointBallNode::get_target_position() const
{
    const Quaterniond & Q = joint_ball.target_q;
    const Quat q( Q.x_, Q.y_, Q.z_, Q.w_ );
    return q;
}

void PbdJointBallNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_spatial_gap", "d" ), &PbdJointBallNode::set_spatial_gap );
    ClassDB::bind_method( D_METHOD( "get_spatial_gap" ),      &PbdJointBallNode::get_spatial_gap, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_angular_gap", "d" ), &PbdJointBallNode::set_angular_gap );
    ClassDB::bind_method( D_METHOD( "get_angular_gap" ),      &PbdJointBallNode::get_angular_gap, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_target_position", "r" ), &PbdJointBallNode::set_target_position );
    ClassDB::bind_method( D_METHOD( "get_target_position" ),      &PbdJointBallNode::get_target_position, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_motor_gap", "d" ), &PbdJointBallNode::set_motor_gap );
    ClassDB::bind_method( D_METHOD( "get_motor_gap" ),      &PbdJointBallNode::get_motor_gap, Variant::REAL );


    ADD_PROPERTY( PropertyInfo( Variant::REAL, "spatial_gap" ), "set_spatial_gap", "get_spatial_gap" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "angular_gap" ), "set_angular_gap", "get_angular_gap" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "target_position" ), "set_target_position", "get_target_position" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "motor_gap" ), "set_motor_gap", "get_motor_gap" );
}






}





