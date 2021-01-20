
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
    //Vector3d d = r_world_prev - r_world;
    // Subtract the projection onto the normal.
    // It leaves us with tangential component.
    //d = d - n_world * d.DotProduct( n_world );
    
    const Vector3d pa = body_a->pose.r + (body_a->pose.q * this->r_a);
    const Vector3d pa_prev = body_a->pose_prev.r + (body_a->pose_prev.q * this->r_a);

    const Vector3d pb = body_b->pose.r + (body_b->pose.q * this->r_b);
    const Vector3d pb_prev = body_b->pose_prev.r + (body_b->pose_prev.q * this->r_b);

    const Vector3d d = (pa - pa_prev) - (pb - pb_prev);

    const Float c = d.Length();
    if (c < EPS)
        return;

    const Vector3d n = d / c;
    const Float mu_a = body_a->specific_inv_mass_pos( this->r_a, n );
    const Float mu_b = body_b->specific_inv_mass_pos( this->r_b, n );
    const Float compliance = (body_a->compliance_tangential + body_b->compliance_tangential) / 2.0;
    Float lambda = this->lambda_tangential;

    const Float alpha_ = compliance / (h*h);
    const Float d_lambda = -(c + alpha_*lambda) / (mu_a + mu_b + alpha_);
    lambda += d_lambda;

    const Float th = body->friction * this->lambda_normal;
    apply_friction = ( std::abs(lambda) > std::abs(th) );
    if ( apply_friction )
        return;

    this->lambda_tangential = lambda;

    const Vector3d p = n * d_lambda;
    if ( body_a->mass > 0.0 )
    {
        const Float inv_m = 1.0 / body_a->mass;
        Vector3d r = body->pose.r;
        Quaterniond q = body->pose.q;
        const Vector3d dr = p * inv_m;
        r += dr;
        body->pose.r = r;

        const Vector3d ra_w = q * this->r_a;
        Vector3d k = ra_w.CrossProduct( p );
        const Matrix3d inv_I = body_a->inv_I();
        k = inv_I * k;
        k *= 0.5;
        Quaterniond dq = Quaterniond( 0.0, k.x_, k.y_, k.z_ );
        dq = dq * q;
        q.w_ += dq.w_;
        q.x_ += dq.x_;
        q.y_ += dq.y_;
        q.z_ += dq.z_;

        q.Normalized();
        body->pose.q = q;
    }
    if ( body_b->mass > 0.0 )
    {
        const Float inv_m = 1.0 / body_b->mass;
        Vector3d r = body_b->pose.r;
        Quaterniond q = body_b->pose.q;
        const Vector3d dr = p * inv_m;
        r -= dr;
        body->pose.r = r;

        const Vector3d rb_w = q * this->r_b;
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
    const Vector3d v_w_a = body_a->vel + body_a->omega.CrossProduct( body_a->pose.q * this->r_a );
    const Vector3d v_w_b = body_b->vel + body_b->omega.CrossProduct( body_b->pose.q * this->r_b );
    const Vector3d v_w  = v_w_a - v_w_b;
    const Float v_w_n   = v_w.DotProduct( this->n_world );
    const Vector3d v_t  = v_w - ( this->n_world * v_w_n );

    if ( apply_friction )
    {
        const Float abs_v_t = v_t.Length();
        if ( abs_v_t > EPS )
        {
            const Float friction = (body_a->friction + body_b->friction) / 2.0;
            Float f_n = friction * this->lambda_normal / h;
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

    // Restitution
    // Project current world velocity onto contact normal.
    const Float v_w_n = v_w.DotProduct( this->n_world );
    // The same projection just before the contact has been processed.
    cosnt Vector3d vel_a = body_a->vel;
    cosnt Vector3d vel_b = body_b->vel;
    Float v_w_n_prev = this->n_world.DotProduct(vel_a - vel_b);
    const Float restitution = (body_a->restitution + body_b->restisution) * 0.5;
    v_w_n_prev = v_w_n_prev * restitution;
    if (v_w_n_prev > 0.0)
        v_w_n_prev = 0.0;
    dv += this->n_world * ( v_w_n + v_w_n_prev );

    // Applying "dv".
    const Float abs_dv = dv.Length();
    if ( abs_dv > EPS )
    {
        if ( ( body_a->mass > 0.0) || (body_b->mass > 0.0) )
        {
            const Vector3d n = dv / abs_dv;
            const Float mu_a = body_a->specific_inv_mass_pos( this->r_a, n ); // 1.0 / body->mass;
            const Float mu_b = body_b->specific_inv_mass_pos( this->r_b, n ); // 1.0 / body->mass;
            const Vector3d p = dv/(mu_a + mu_b);
            if ( body_a->mass > 0.0 )
            {
                const Float m = body_a->mass;
                body_a->vel += p / m;

                const Matrix3d inv_I = body_a->inv_I();
                const Vector3d r_w = body_a->pose.q * this->r_a;
                body_a->omega += inv_I * ( r_w.CrossProduct( p ) );
            }
            if ( body_b->mass > 0.0 )
            {
                const Float m = body_b->mass;
                body_b->vel -= p / m;

                const Matrix3d inv_I = body_b->inv_I();
                const Vector3d r_w = body_b->pose.q * this->r_b;
                body_b->omega -= inv_I * ( r_w.CrossProduct( p ) );
            }
        }
    }

    // Angular damping.
    if ( body_a->mass > 0.0 )
    {
        Float ang_damp = body_a->damping_angular * h;
        if (ang_damp > 1.0)
            ang_damp = 1.0;
        const Vector3d dw = body_a->omega * ang_damp;
        body_a->omega -= dw;
    }
    if ( body_b->mass > 0.0 )
    {
        Float ang_damp = body_b->damping_angular * h;
        if (ang_damp > 1.0)
            ang_damp = 1.0;
        const Vector3d dw = body_b->omega * ang_damp;
        body_b->omega -= dw;
    }
}



}







