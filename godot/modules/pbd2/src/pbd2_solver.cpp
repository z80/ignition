
#include "pbd3_solver.h"


namespace Pbd
{

namespace Solver
{

Float correct_position( Float h, Float compliance, Float c, const Vector3d & n, Float lambda, const Vector3d & r_a, RigidBody * body_a, const Vector3d & r_b, RigidBody * body_b )
{
    const Float mu_a = (body_a != nullptr) ? body_a->specific_inv_mass_pos( r_a, n ) : 0.0;
    const Float mu_b = (body_b != nullptr) ? body_b->specific_inv_mass_pos( r_b, n ) : 0.0;

    const Float alpha_ = compliance / (h*h);
    const Float d_lambda = -(c + alpha_*lambda) / (mu_a + mu_b + alpha_);
    lambda += d_lambda;
    const Vector3d p = n * d_lambda;

    if ( (body_a != nullptr) && (body_a->mass > 0.0) )
    {
        const Float inv_m   = 1.0 / body_a->mass;
        const Vector3d r    = body_a->pose.r;
        const Quaterniond q = body_a->pose.q;
        const Vector3d dr = p * inv_m;
        body_a->pose.r += dr;

        const Vector3d ra_w = body_a->pose.q * r_a;
        Vector3d k = ra_w.Cross( p );
        const Matrix3d inv_I = body_a->inv_I();
        k = inv_I * k;
        k *= 0.5;
        const Quaterniond dq( 0.0, k.x_, k.y_, k.z_ );
        dq = dq * q;
        q += dq;
        q.Normalize();
        body_a->pose.q = q;
    }

    if ( (body_b != nullptr) && (body_b->mass > 0.0) )
    {
        const Float inv_m   = 1.0 / body_b->mass;
        const Vector3d r    = body_b->pose.r;
        const Quaterniond q = body_b->pose.q;
        const Vector3d dr = p * inv_m;
        body_b->pose.r += dr;

        const Vector3d ra_w = body_b->pose.q * r_a;
        Vector3d k = ra_w.Cross( p );
        const Matrix3d inv_I = body_b->inv_I();
        k = inv_I * k;
        k *= 0.5;
        const Quaterniond dq( 0.0, k.x_, k.y_, k.z_ );
        dq = dq * q;
        q += dq;
        q.Normalize();
        body_b->pose.q = q;
    }
    
    return lambda;
}

Float correct_rotation( Float h, Float compliance, Float theta, const Vector3d & n, Float lambda, RigidBody * body_a, RigidBoy * body_b )
{
    const Float mu_a = (body_a != nullptr) ? body_a->specific_inv_mass_rot( n ) : 0.0;
    const Float mu_b = (body_b != nullptr) ? body_b->specific_inv_mass_rot( n ) : 0.0;
    const Float alpha_ = compliance / (h*h);
    const Float d_lambda = -(theta + alpha_*lambda)/(mu__a + mu_b + alpha_);
    lambda += d_lambda;
    const Vector3d p = n * d_lambda;

    if ( (body_a != nullptr) && (bosy_a->mass > 0.0) )
    {
        const Quaterniond q = body_a->pose.q;
        const Matrix3d inv_I = body_a->inv_I();
        Vector3d k = inv_I * p;
    }
}

}



}








