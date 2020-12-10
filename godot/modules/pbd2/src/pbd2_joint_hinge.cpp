
#include "pbd2_joint_hinge.h"
#include "pbd2_rigid_body.h"
#include "pbd2_solver.h"

namespace Pbd
{


JointHinge::JointHinge()
    : Joint()
{
}

JointHinge::~JointHinge()
{
}

void JointHinge::init_lambdas()
{
    lambdas.resize( 3 );
    lambdas.ptrw()[0] = 0.0;
    lambdas.ptrw()[1] = 0.0;
    lambdas.ptrw()[2] = 0.0;
}

void JointHinge::init_motor_target()
{
    if ( ( body_a == nullptr ) || ( body_b == nullptr ) )
        return;

    const Vector3d e1a_w = body_a->pose.q * e1_a;
    const Vector3d e2a_w = body_a->pose.q * e2_a;
    const Vector3d e1b_w = body_b->pose.q * e1_b;
    const Vector3d e2b_w = body_b->pose.q * e2_b;

    // First rotation "e1b" towards "e1a".
    const Vector3d n = e1b_w.CrossProduct( e1a_w );
    const Float si = n.Length();
    Quaterniond q1;
    if ( si < EPS )
        q1 = Quaterniond::IDENTITY;
    else
    {
        const Float angle = std::asin( si );
        const Float angle_2 = angle * 0.5;
        const Float si2 = std::sin( angle_2 );
        const Float co2 = std::cos( angle_2 );
        const Vector3d nn = n / si;
        q1 = Quaterniond( co2, nn.x_*si2, nn.y_*si2, nn.z_*si2 );
    }

    const Vector3d e2b_w_2 = q1 * e2b_w;
    const Vector3d n2 = e2a_w.CrossProduct( e2b_w_2 );
    const Float si_t = n2.Length();
    const Float co_t = e2b_w_2.DotProduct( e2a_w );

    target_position = -atan2( si_t, co_t );
}

void JointHinge::solver_step( Float h )
{
    lambdas.ptrw()[0] = solver_step_position( lambdas.ptr()[0], h );
    lambdas.ptrw()[1] = solver_step_rotation( lambdas.ptr()[1], h );
    lambdas.ptrw()[2] = solver_step_motor( lambdas.ptr()[2], h );
}

Float JointHinge::solver_step_position( Float lambda, Float h )
{
    const Vector3d dr = _delta_r();
    Float c = dr.Length();
    if ( c <= spatial_gap )
        return lambda;

    const Vector3d n = dr / c;
    c -= spatial_gap;

    const Float ret = Solver::correct_position( h, compliance_joint, c, n, lambda, at_a, body_a, at_b, body_b );
    return ret;
}

Float JointHinge::solver_step_rotation( Float lambda, Float h )
{
    const Vector3d dtheta = _delta_theta();
    Float theta = dtheta.Length();
    if ( theta <= angular_gap )
        return lambda;

    const Vector3d n = dtheta / theta;
    theta -= angular_gap;

    const Float ret = Solver::correct_rotation( h, compliance_joint, theta, n, lambda, body_a, body_b );
    return ret;
}

Float JointHinge::solver_step_motor( Float lambda, Float h )
{
    if ( !motor )
        return lambda;

    if ( !position_control )
    {
        target_position += h * target_velocity;
        if ( target_position > _2_PI )
            target_position -= _2_PI;
        else if ( target_position < -_2_PI )
            target_position += _2_PI;
    }

    const Vector3d dtheta = _delta_motor();
    Float theta = dtheta.Length();
    if ( theta <= motor_gap )
        return lambda;

    const Vector3d n = dtheta / theta;
    theta -= motor_gap;

    const Float ret = Solver::correct_rotation( h, compliance_motor, theta, n, lambda, body_a, body_b );
    return ret;
}



Vector3d JointHinge::_delta_r() const
{
    const Vector3d ra_w = _world_r( body_a, at_a );
    const Vector3d rb_w = _world_r( body_b, at_b );
    const Vector3d dr = ra_w - rb_w;
    return dr;
}

Vector3d JointHinge::_delta_theta() const 
{
    const Vector3d ra_w = body_a->pose.q * e1_a;
    const Vector3d rb_w = body_b->pose.q * e1_b;
    const Vector3d dtheta = rb_w.CrossProduct( ra_w );
    return dtheta;
}

Vector3d JointHinge::_world_r( RigidBody * body, const Vector3d & r ) const
{
    const Vector3d rw = (body->pose.q * r) + body->pose.r;
    return rw;
}

Vector3d JointHinge::_delta_motor() const
{
    const Vector3d e1a_w = body_a->pose.q * e1_a;
    const Vector3d e2a_w = body_a->pose.q * e2_a;
    const Quaterniond quat = Quaterniond( target_position, e1a_w );
    const Vector3d e2_target = quat * e2a_w;
    const Vector3d e2b_w = body_b->pose.q * e2_b;
    const Vector3d dtheta = e2b_w.CrossProduct( e2_target );
    return dtheta;
}







}



