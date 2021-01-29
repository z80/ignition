
#include "pbd2_contact_point_bb.h"
#include "pbd2_rigid_body.h"

namespace Pbd
{

const Float ContactPointBb::EPS = 0.0001;

ContactPointBb::ContactPointBb()
{
    body_a = nullptr;
    body_b = nullptr;
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

		body_a = inst.body_a;
		body_b = inst.body_b;
    }
    return * this;
}

    
void ContactPointBb::init_lambdas()
{
    lambda_normal = 0.0;
    lambda_tangential = 0.0;
}

void ContactPointBb::solve_normal( RigidBody * body_a, RigidBody * body_b, Float h )
{
	const Float w_a = body_a->specific_inv_mass_pos( r_a, n_world );
	const Float w_b = body_b->specific_inv_mass_pos( r_b, n_world );

	const Float w_both = w_a + w_b;

	const Float compliance_a = body_a->compliance_normal;
	const Float compliance_b = body_b->compliance_normal;
	const Float compliance_normal = (compliance_a + compliance_b) / 2.0;

	const Float alpha_ = compliance_normal / (h*h);
	const Float lambda = lambda_normal;
	const Float d_lambda = (depth + alpha_*lambda) / (w_both + alpha_);
	lambda_normal += d_lambda;

	const Vector3d d = d_lambda * n_world;

	// Body "a".
	// Plus for body "a".
	if ( body_a->mass > 0.0 )
	{
		const Vector3d dr = d / body_a->mass;
		const Matrix3d inv_I = body_a->inv_I();
		const Pose & pose = body_a->pose;
		const Vector3d r_world = pose.q * r_a;
		const Vector3d r_x_d = r_world.CrossProduct( d );
		const Vector3d rot_2 = inv_I * r_x_d * 0.5;
		Quaterniond dq( 0.0, rot_2.x_, rot_2.y_, rot_2.z_ );
		dq = dq * pose.q;
		Quaterniond q = pose.q;
		q.w_ += dq.w_;
		q.x_ += dq.x_;
		q.y_ += dq.y_;
		q.z_ += dq.z_;
		q.Normalize();

		body_a->pose.r += dr;
		body_a->pose.q  = q;
	}

	// Body "b".
	// And minus for body "b".
	if ( body_b->mass > 0.0 )
	{
		const Vector3d dr = d / body_b->mass;
		const Pose & pose = body_b->pose;
		const Matrix3d inv_I = body_b->inv_I();
		const Vector3d r_world = pose.q * r_b;
		const Vector3d r_x_d = r_world.CrossProduct( d );
		const Vector3d rot_2 = inv_I * r_x_d * 0.5;
		Quaterniond dq( 0.0, rot_2.x_, rot_2.y_, rot_2.z_ );
		dq = dq * pose.q;
		Quaterniond q = pose.q;
		q.w_ -= dq.w_;
		q.x_ -= dq.x_;
		q.y_ -= dq.y_;
		q.z_ -= dq.z_;
		q.Normalize();

		body_b->pose.r -= dr;
		body_b->pose.q  = q;
	}
}

void ContactPointBb::solve_tangential( RigidBody * body_a, RigidBody * body_b, Float h )
{
    //Vector3d d = r_world_prev - r_world;
    // Subtract the projection onto the normal.
    // It leaves us with tangential component.
    //d = d - n_world * d.DotProduct( n_world );
    
    const Vector3d pa = body_a->orig_pose.r + (body_a->orig_pose.q * this->r_a);
    const Vector3d pa_prev = body_a->prev_pose.r + (body_a->prev_pose.q * this->r_a);

    const Vector3d pb = body_b->orig_pose.r + (body_b->orig_pose.q * this->r_b);
    const Vector3d pb_prev = body_b->prev_pose.r + (body_b->prev_pose.q * this->r_b);

    Vector3d d = (pa - pa_prev) - (pb - pb_prev);
	const Float normal_proj = n_world.DotProduct( d );
	d = d - (n_world * normal_proj);

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

    const Float friction = (body_a->friction + body_b->friction) / 2.0;
    const Float th = friction * this->lambda_normal;
    apply_friction = ( std::abs(lambda) > std::abs(th) );
    if ( apply_friction )
        return;

    this->lambda_tangential = lambda;

    const Vector3d p = n * d_lambda;
    if ( body_a->mass > 0.0 )
    {
        const Float inv_m = 1.0 / body_a->mass;
        Vector3d r = body_a->pose.r;
        Quaterniond q = body_a->pose.q;
        const Vector3d dr = p * inv_m;
        r += dr;
        body_a->pose.r = r;

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
        body_a->pose.q = q;
    }
    if ( body_b->mass > 0.0 )
    {
        const Float inv_m = 1.0 / body_b->mass;
        Vector3d r = body_b->pose.r;
        Quaterniond q = body_b->pose.q;
        const Vector3d dr = p * inv_m;
        r -= dr;
        body_b->pose.r = r;

        const Vector3d rb_w = q * this->r_b;
        Vector3d k = rb_w.CrossProduct( p );
        const Matrix3d inv_I = body_b->inv_I();
        k = inv_I * k;
        k *= 0.5;
        Quaterniond dq = Quaterniond( 0.0, k.x_, k.y_, k.z_ );
        dq = dq * q;
        q.w_ -= dq.w_;
        q.x_ -= dq.x_;
        q.y_ -= dq.y_;
        q.z_ -= dq.z_;

        q.Normalized();
        body_b->pose.q = q;
    }
}

void ContactPointBb::solve_dynamic_friction( RigidBody * body_a, RigidBody * body_b, Float h )
{
    Vector3d dv = Vector3d::ZERO;
    
    // World velocity of this point.
    const Vector3d v_w_a = body_a->vel + body_a->omega.CrossProduct( body_a->pose.q * this->r_a );
    const Vector3d v_w_b = body_b->vel + body_b->omega.CrossProduct( body_b->pose.q * this->r_b );
    const Vector3d v_w  = v_w_a - v_w_b;
    // Project current world velocity onto contact normal.
    const Float v_w_n   = v_w.DotProduct( this->n_world );
    // Subtract normal velocity component and getting tangential one.
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
    const Float damping_linear = (body_a->damping_linear + body_b->damping_linear) / 2.0;
    Float lin_damp = damping_linear * h;
    if (lin_damp > 1.0)
        lin_damp = 1.0;
    dv -= v_t * lin_damp;

    // Restitution
    // The same projection just before the contact has been processed.
	const Vector3d vel_a = body_a->orig_vel + body_a->omega.CrossProduct( body_a->orig_pose.q * this->r_a );
    const Vector3d vel_b = body_b->orig_vel + body_b->omega.CrossProduct( body_b->orig_pose.q * this->r_b );
    Float v_w_n_prev = this->n_world.DotProduct(vel_a - vel_b);
    const Float restitution = (body_a->restitution + body_b->restitution) * 0.5;
    if (v_w_n_prev > 0.0)
        v_w_n_prev = 0.0;
    //dv += this->n_world * ( v_w_n_prev - v_w_n * restitution );
	dv -= this->n_world * ( v_w_n + v_w_n_prev * restitution );

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
                body_a->vel += (p / m);

                const Matrix3d inv_I = body_a->inv_I();
                const Vector3d r_w = body_a->pose.q * this->r_a;
                body_a->omega += inv_I * ( r_w.CrossProduct( p ) );
            }
            if ( body_b->mass > 0.0 )
            {
                const Float m = body_b->mass;
                body_b->vel -= (p / m);

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







