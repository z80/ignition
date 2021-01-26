
#include "pbd2_joint_ball.h"
#include "pbd2_rigid_body.h"
#include "pbd2_solver.h"

namespace Pbd
{


JointBall::JointBall()
    : Joint()
{
	spatial_gap = 0.0;
	angular_gap = 0.0;
	motor_gap   = 0.0;
	position_control = true;
}

JointBall::~JointBall()
{
}

void JointBall::init_lambdas()
{
    lambdas.resize( 4 );
    lambdas.ptrw()[0] = 0.0;
    lambdas.ptrw()[1] = 0.0;
    lambdas.ptrw()[2] = 0.0;
    lambdas.ptrw()[3] = 0.0;
}

void JointBall::init_motor_target()
{
	if ( (body_a != nullptr) && (body_b != nullptr) )
	{
		const Quaterniond q_a = body_a->pose.q;
		const Quaterniond q_b = body_b->pose.q;
		target_q = q_b.Inverse() * q_a;
	}
}

void JointBall::solver_step( Float h )
{
    if ( motor )
        solver_step_motor( h );
    lambdas.ptrw()[0] = solver_step_position( lambdas.ptrw()[0], h );
}


Float JointBall::solver_step_position( Float lambda, Float h )
{
    const Vector3d dr = _delta_r();
    Float c = dr.Length();
    if ( (c <= spatial_gap) || (c < EPS) )
        return lambda;

    const Vector3d n = dr / c;
    c -= spatial_gap;

    const Float ret = Solver::correct_position( h, compliance_joint, c, n, lambda, at_a, body_a, at_b, body_b );
    return ret;
}

void JointBall::solver_step_motor( Float h )
{
	if ( !position_control )
	{
		const Float k = 0.5*h;
		Quaterniond dq( 0.0, target_w.x_*k, target_w.y_*k, target_w.z_*k );
		dq = dq * target_q;
		target_q.w_ += dq.w_;
		target_q.x_ += dq.x_;
		target_q.y_ += dq.y_;
		target_q.z_ += dq.z_;
		target_q.Normalize();
	}

    Vector3d d_angle_3 = _delta_motor_eta();
    Float abs_angle = d_angle_3.Length();
    if ( (abs_angle > motor_gap) && (abs_angle > EPS) )
    {
        const Vector3d n = d_angle_3 / abs_angle;
        abs_angle -= motor_gap;
        lambdas.ptrw()[1] = Solver::correct_rotation( h, compliance_motor, abs_angle, n, lambdas.ptr()[1], body_a, body_b );
    }

    d_angle_3 = _delta_motor_theta();
    abs_angle = d_angle_3.Length();
    if ( (abs_angle > motor_gap) && (abs_angle > EPS) )
    {
        const Vector3d n = d_angle_3 / abs_angle;
        abs_angle -= motor_gap;
        lambdas.ptrw()[2] = Solver::correct_rotation( h, compliance_motor, abs_angle, n, lambdas.ptr()[2], body_a, body_b );
    }

    d_angle_3 = _delta_motor_zeta();
    abs_angle = d_angle_3.Length();
    if ( (abs_angle > motor_gap) && (abs_angle > EPS) )
    {
        const Vector3d n = d_angle_3 / abs_angle;
        abs_angle -= motor_gap;
        lambdas.ptrw()[3] = Solver::correct_rotation( h, compliance_motor, abs_angle, n, lambdas.ptr()[3], body_a, body_b );
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
    const Vector3d rw = (body->pose.q * r) + body->pose.r;
    return rw;
}

Vector3d JointBall::_delta_motor_eta() const
{
    const Vector3d ra_w = body_a->pose.q * e1_a;
    const Vector3d rb_w = (body_b->pose.q * target_q) * e1_b;
    const Vector3d angle_3 = rb_w.CrossProduct( ra_w );
    return angle_3;
}

Vector3d JointBall::_delta_motor_theta() const
{
    const Vector3d ra_w = body_a->pose.q * e2_a;
    const Vector3d rb_w = (body_b->pose.q * target_q) * e2_b;
    const Vector3d angle_3 = rb_w.CrossProduct( ra_w );
    return angle_3;
}

Vector3d JointBall::_delta_motor_zeta() const
{
    const Vector3d ra_w = body_a->pose.q * e3_a;
    const Vector3d rb_w = (body_b->pose.q * target_q) * e3_b;
    const Vector3d angle_3 = rb_w.CrossProduct( ra_w );
    return angle_3;
}




}







