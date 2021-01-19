
#include "pbd2_contact_point_bb.h"
#include "pbd2_rigid_body.h"

namespace Pbd
{

const Float ContactPointBb::EPS = 0.0001;

ContactPointBb::ContactPointBb()
{
    in_contact = false;
    in_contact_next = false;
    depth = 0.0;
    apply_friction = false;
    init_lambdas();
}

ContactPointBb::~ContactPointBb()
{
}

ContactPointBb::ContactPointBb( const ContactPointBb & inst )
{
    *this = inst;
}

const ContactPointBb & ContactPointBb::operator=( const ContactPointBb & inst )
{
    if ( this != &inst )
    {
        r_a      = inst.r_a;
        r_b      = inst.r_b;
        n_world  = inst.n_world;
        depth    = inst.depth;

        lambda_normal     = inst.lambda_normal;
        lambda_tangential = inst.lambda_tangential;
        
        apply_friction  = inst.apply_friction;
    }
    return * this;
}

    
void ContactPointBb::init_lambdas()
{
    lambda_normal = 0.0;
    lambda_tangential = 0.0;
}

void ContactPointBb::solve_tangential( RigidBody * body_a, RigidBody * body_b, Float h )
{
    const bool in_contact = this->in_contact;
    if ( !in_contact )
        return;

    Vector3d d = r_world_prev - r_world;
    // Subtract the projection onto the normal.
    // It leaves us with tangential component.
    d = d - n_world * d.DotProduct( n_world );

    const Float c = d.Length();
    if (c < EPS)
        return;

    const Vector3d n = d / c;
    const Float mu_b = body->specific_inv_mass_pos( this->r, n );
    const Float compliance = body->compliance_tangential;
    Float lambda = this->lambda_tangential;

    const Float alpha_ = compliance / (h*h);
    const Float d_lambda = -(c + alpha_*lambda) / (mu_b + alpha_);
    lambda += d_lambda;

    const Float th = body->friction * this->lambda_normal;
	apply_friction = ( std::abs(lambda) > std::abs(th) );
    if ( apply_friction )
        return;

    this->lambda_tangential = lambda;

    const Vector3d p = n * d_lambda;
    if ( body->mass > 0.0 )
    {
        const Float inv_m = 1.0 / body->mass;
        Vector3d r = body->pose.r;
        Quaterniond q = body->pose.q;
        const Vector3d dr = p * inv_m;
        r -= dr;
        body->pose.r = r;

        const Vector3d rb_w = q * this->r;
        Vector3d k = rb_w.CrossProduct( p );
        const Matrix3d inv_I = body->inv_I();
        k = inv_I * k;
        k *= 0.5;
        Quaterniond dq = Quaterniond( 0.0, k.x_, k.y_, k.z_ );
        dq = dq * q;
        q.w_ -= dq.w_;
        q.x_ -= dq.x_;
        q.y_ -= dq.y_;
        q.z_ -= dq.z_;

        q.Normalized();
        body->pose.q = q;
    }
}

void ContactPointBb::solve_dynamic_friction( RigidBody * body_a, RigidBody * body_b, Float h )
{
    Vector3d dv = Vector3d::ZERO;
    
    // World velocity of this point.
    const Vector3d v_w = body->vel + body->omega.CrossProduct( body->pose.q * this->r );
    const Vector3d v_n = -( this->n_world * (v_w.DotProduct(this->n_world)) );
    const Vector3d v_t = v_w + v_n;

    if ( apply_friction )
    {
        const Float abs_v_t = v_t.Length();
        if ( abs_v_t > EPS )
        {
            Float f_n = body->friction * this->lambda_normal / h;
            if ( f_n > abs_v_t )
                f_n = abs_v_t;
            dv = -( v_t * (f_n / abs_v_t) );
        }
    }

    // Linear damping.
    Float lin_damp = body->damping_linear * h;
    if (lin_damp > 1.0)
        lin_damp = 1.0;
    dv += v_t * lin_damp;

    const Vector3d r_w = body->pose.q * this->r;
    // Restitution
    // Project current world velocity onto contact normal.
    const Float v_w_n = v_w.DotProduct( this->n_world );
    // The same projection just before the contact has been processed.
    Float v_w_n_prev = this->v_world.DotProduct( this->n_world );
    v_w_n_prev = v_w_n_prev * body->restitution;
    if (v_w_n_prev > 0.0)
        v_w_n_prev = 0.0;
    dv += this->n_world * ( v_w_n + v_w_n_prev );

    // Applying "dv".
    const Float abs_dv = dv.Length();
    if ( abs_dv > EPS )
    {
        const Vector3d n = dv / abs_dv;
                const Float mu_b = body->specific_inv_mass_pos( this->r, n ); // 1.0 / body->mass;
        const Vector3d p = dv/mu_b;
        const Float m = body->mass;
        body->vel -= p / m;

        const Matrix3d inv_I = body->inv_I();
        body->omega -= inv_I * ( r_w.CrossProduct( p ) );
    }

    // Angular damping.
    Float ang_damp = body->damping_angular * h;
    if (ang_damp > 1.0)
        ang_damp = 1.0;
    const Vector3d dw = body->omega * ang_damp;
    body->omega -= dw;
}



}







