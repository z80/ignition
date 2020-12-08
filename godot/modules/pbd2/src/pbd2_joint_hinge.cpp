
#include "pbd2_joint_hinge.h"

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
    lambdas.wptr()[0] = 0.0;
    lambdas.wptr()[1] = 0.0;
    lambdas.wptr()[2] = 0.0;
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
    const Vector3d n = e1b_w.Cross( e1a_w );
    const Float si = n.Length();
    Quaterniond q1;
    if ( si < EPS ):
        q1 = Quaterniond::IDENTITY;
    else
    {
        const Float angle = std::asin( si )
        const Float angle_2 = angle * 0.5;
        const Float si2 = std::sin( angle_2 );
        const Float co2 = std::cos( angle_2 );
        const Vector3 nn = n / si;
        q1 = Quaterniond( co2, nn.x_*si2, nn.y_*si2, nn.z_*si2 );
    }

    const Vector3d e2b_w_2 = q1 * e2b_w;
    const Vector3d n2 = e2a_w.Cross( e2b_w_2 );
    const Float si = n2.Length();
    const Float co = e2b_w_2.Dot( e2a_w );

    target_position = -atan2( si, co );
}

void JointHinge::solver_step( Float h )
{
    lambdas.wptr()[0] = solver_step_position( lambdas.ptr()[0], h );
    lambdas.wptr()[1] = solver_step_rotation( lambdas.ptr()[1], h );
    lambdas.wptr()[2] = solver_step_motor( lambdas.ptr()[2], h );
}

Float JointHinge::solver_step_position( Float lambda, Float h )
{
    const Vector3d dr = _delta_r();
    Flaot c = dr.Length();
    if ( c <= spatial_gap )
        return lambda;

    const Vector3d n = dr / c;
    c -= spatial_gap;

    const Float ret = Solvers::correct_position( h, compliance_joint, c, n, lambda, at_a, body_a, at_b, body_b );
    return ret;
}

Float JointHinge::solver_step_rotation( Float lambda, Float h )
{
    const Vector3 dtheta = _delta_theta();
    const Float theta = dtheta.Length();
    if ( theta <= angular_gap )
        return lambda;

    const Vector3d n = dtheta / theta;
    theta -= angular_gap;

    const Float ret = Solvers::correct_rotation( h, compliance_joint, theta, n, lambda, body_a, body_b );
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

    const Float ret = Solvers::correct_rotation( h, compliance_motor, theta, n, lambda, body_a, body_b );
    return ret;
}



Float JointHinge::_delta_r() const
{
    const Vector3d ra_w = _world_r( body_a, at_a );
    const Vector3d rb_w = _world_r( body_b, at_b );
    const Vector3d dr = ra_w - rb_w;
    return dr;
}

Float JointHinge::_delta_theta() const 
{
    const Vector3d ra_w = body_a->pose.q * e1_a;
    const Vector3d rb_w = body_b->pose.q * e1_b;
    const Vector3d dtheta = rb_w.Cross( ra_w );
    return dtheta;
}

Vector3d JointHinge::_world_r( RigidBody * body, const Vector3d & r ) const
{
    const Vector3d rw = (body->pose.q * r) + body->pose.r;
    return rw;
}

Float JointHinge::_delta_motor() const
{
    const Vector3d e1a_w = body_a->pose.q * e1_a;
    const Vector3d e1b_w = body_b->pose.q * e1_b;
    const Quaterniond quat = Quaterniond( targe_position, e1a_w );
    const Vector3d e2_target = quat * e2a_w;
    const Vector3d e2b_w = body_b->pose.q * e2_b;
    const Vector3d dtheta = e2b_w.Cross( e2_target );
    return dtheta;
}







}



