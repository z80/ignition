
#include "pbd2_rigid_body.h"


namespace Pbd
{


RigidBody::RigidBody()
{
    mass = 1.0;
    inertia     = Matrix3d::IDENTITY;
    inv_inertia = Matrix3d::IDENTITY;

	friction        = 1.0;
	damping_linear  = 0.1;
	damping_angular = 0.1;
	restitution     = 0.0;;

	compliance_normal     = 0.0;
	compliance_tangential = 0.0;
}

RigidBody::~RigidBody()
{
}

RigidBody::RigidBody( const RigidBody & inst )
{
    *this = inst;
}

const RigidBody & RigidBody::operator=( const RigidBody & inst )
{
    if ( this != &inst )
    {
        pose      = inst.pose;
        prev_pose = inst.prev_pose;
        orig_pose = inst.orig_pose;
        vel       = inst.vel;
        omega     = inst.omega;

        mass        = inst.mass;
        inertia     = inst.inertia;
        inv_inertia = inst.inv_inertia;

        force       = inst.force;
        torque      = inst.torque;

		friction         = inst.friction;
		damping_linear   = inst.damping_linear;
		damping_angular  = inst.damping_angular;
		restitution      = inst.restitution;

		compliance_normal     = inst.compliance_normal;
		compliance_tangential = inst.compliance_tangential;
        
        contact_points = inst.contact_points;
    }
    return *this;
}

void RigidBody::set_inertia( const Matrix3d & I )
{
    inertia = I;
    inv_inertia = I.Inverse();
}

Matrix3d RigidBody::I() const
{
    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d i = A * inertia * A.Transpose();
    return i;
}

Matrix3d RigidBody::inv_I() const
{
    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d inv_i = A.Transpose() * inv_inertia * A;
    return inv_i;
}


void RigidBody::integrate_dynamics( Float h )
{
	if ( mass <= 0.0 )
		return;

    // Save previous pose.
    prev_pose = pose;

    // Translation.
    vel += (force * h) / mass;
    pose.r += vel * h;

    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d I = A * inertia * A.Transpose();
    const Matrix3d inv_I = A.Transpose() * inv_inertia * A;

    const Vector3d w_x_Iw = omega.CrossProduct( I * omega );
    const Vector3d dw = ( inv_I * ( torque - w_x_Iw ) ) * h;
    omega += dw;

    const Float k = 0.5 * h;
    Quaterniond dq( 0.0, omega.x_*k, omega.y_*k, omega.z_*k );
	dq = dq * pose.q;
    pose.q.w_ += dq.w_;
	pose.q.x_ += dq.x_;
	pose.q.y_ += dq.y_;
	pose.q.z_ += dq.z_;
    pose.q.Normalize();
}

void RigidBody::update_velocities( Float h )
{
	if ( mass <= 0.0 )
		return;

    this->vel = ( pose.r - prev_pose.r ) / h;
    const Quaterniond dq = pose.q * prev_pose.q.Conjugate();
    Vector3d w = Vector3d( dq.x_, dq.y_, dq.z_ );
    if ( dq.w_ < 0.0 )
        w = -w;

    w *= 2.0 / h;
    this->omega = w;
}

void RigidBody::init_contact_lambdas()
{
    const int qty = contact_points.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPoint & pt = contact_points.ptrw()[i];
        pt.init_lambdas();
    }
}

void RigidBody::solve_contacts( Float h )
{
    const int qty = contact_points.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPoint & pt = contact_points.ptrw()[i];
        pt.solve( this, h );
    }
}

void RigidBody::update_contact_velocities( Float h )
{
    const int qty = contact_points.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPoint & pt = contact_points.ptrw()[i];
        pt.solve_dynamic_friction( this, h );
    }
}

void RigidBody::update_contact_positions()
{
	const int qty = contact_points.size();
	for ( int i=0; i<qty; i++ )
	{
		ContactPoint & pt = contact_points.ptrw()[i];
		pt.update_prev();
	}
}


// "r" local, "n" world.
Float RigidBody::specific_inv_mass_pos( const Vector3d & r, const Vector3d & n )
{
    if ( mass <= 0.0 )
        return 0.0;

    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d inv_I = A.Transpose() * inv_inertia * A;
    const Vector3d r_world = pose.q * r;
    const Vector3d r_x_n = r_world.CrossProduct( n );
    const Float mu = 1.0/mass + r_x_n.DotProduct( inv_I * r_x_n );

    return mu;
}

// "n" world.
Float RigidBody::specific_inv_mass_rot( const Vector3d & n )
{
    if ( mass <= 0.0 )
        return 0.0;

    const Matrix3d A = pose.q.RotationMatrix();
    const Matrix3d inv_I = A.Transpose() * inv_inertia * A;
    const Float mu = n.DotProduct( inv_I * n );

    return mu;
}


}

