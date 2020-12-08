
#include "pbd_joint_ball.h"


namespace Pbd
{


JointBall::JointBall()
    : Joint()
{
}

JointBall::~JointBall()
{
}

void JointBall::init_lambdas()
{
    lambdas.resize( 4 );
    lambdas.wptr()[0] = 0.0;
    lambdas.wptr()[1] = 0.0;
    lambdas.wptr()[2] = 0.0;
    lambdas.wptr()[3] = 0.0;
}

void JointBall::init_motor_target()
{
    const Vector3d q_a = body_a->pose.q;
    const Vector3d q_b = body_b->pose.q;
    target_q = q_b.Inverse() * q_a;
}

void JointBall::solver_step( Float h )
{
    if motor:
        solver_step_motor( lambdas, h )
    lambdas.wptr()[0] = solver_step_position( lambdas.ptr()[0], h );
}


Float JointBall::solver_step_position( Float lambda, Float h )
{
    const Vector3 dr = _delta_r();
    const Float c = dr.Length();
    if ( c <= spatial_gap )
        return lambda;

    const Vector3d n = dr / c
    c -= spatial_gap;

    const Float ret = Solvers::correct_translation( h, compliance, c, n, lambda, at_a, bosy_a, at_b, body_b );
    return ret;
}

Float JointBall::solver_step_motor( Float lambda, Float h )
{
    Vector3 d_angle_3 = _delta_motor_eta();
    Float abs_angle = d_angle_3.Length();
    if ( abs_angle > motor_gap )
    {
        const Vector3d n = d_angle_3 / abs_angle
        abs_angle -= motor_gap
        lambdas.wptr()[1] = Solver::correct_rotation( h, compliance_motor, abs_angle, n, lambdas.ptr()[1], body_a, body_b );
    }

    d_angle_3 = _delta_motor_theta();
    abs_angle = d_angle_3.Length();
    if ( abs_angle > motor_gap )
    {
        const Vector3d n = d_angle_3 / abs_angle
        abs_angle -= motor_gap
        lambdas.wptr()[2] = Solver::correct_rotation( h, compliance_motor, abs_angle, n, lambdas.ptr()[2], body_a, body_b );
    }

    d_angle_3 = _delta_motor_zeta();
    abs_angle = d_angle_3.Length();
    if ( abs_angle > motor_gap )
    {
        const Vector3d n = d_angle_3 / abs_angle
        abs_angle -= motor_gap
        lambdas.wptr()[3] = Solver::correct_rotation( h, compliance_motor, abs_angle, n, lambdas.ptr()[3], body_a, body_b );
    }
}


Vector3d JointBall::_delta_r() const
{
    const Vector3d ra_w = _world_r( body_a, at_a );
    const Vector3d rb_w = _world_r( body_b, at_b );
    const Vector3d dr = ra_w - rb_w;
    return dr;
}

Vector3d JointBall::_world_r( RigidBody * body, const Vector3d & r ) const
{
    const Vector3d rw = (body->pose.q * r) + pbdy->pose.r;
    return rw;
}

Float JointBall::_delta_motor_eta() const
{
    const Vector3d ra_w = body_a->pose.q * e1_a;
    const Vector3d rb_w = (body_b->pose.q * target_q) * e1_b;
    const Vector3d angle_3 = r_b_w.Cross( ra_w );
    return angle_3;
}

Float JointBall::_delta_motor_theta() const
{
    const Vector3d ra_w = body_a->pose.q * e2_a;
    const Vector3d rb_w = (body_b->pose.q * target_q) * e2_b;
    const Vector3d angle_3 = r_b_w.Cross( ra_w );
    return angle_3;
}

Float JointBall::_delta_motor_zeta() const
{
    const Vector3d ra_w = body_a->pose.q * e3_a;
    const Vector3d rb_w = (body_b->pose.q * target_q) * e3_b;
    const Vector3d angle_3 = r_b_w.Cross( ra_w );
    return angle_3;
}




}







