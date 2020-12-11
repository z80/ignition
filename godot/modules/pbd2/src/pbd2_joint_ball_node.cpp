
#include "pbd2_joint_ball_node.h"


namespace Pbd
{

PbdJointBallNode::PbdJointBallNode()
    : PbdJoint()
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

real_t PbdJointBallNode::get_angilar_gap() const
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
    const Quateriond & Q = joint_ball.target_q;
    const Quat q( Q.x_, Q.y_, Q.z_,  )
}

void PbdJointBallNode::_bind_methods()
{
}






}





