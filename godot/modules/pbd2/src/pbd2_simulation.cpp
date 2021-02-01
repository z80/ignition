
#include "pbd2_simulation.h"
#include "pbd2_rigid_body.h"
#include "pbd2_joint.h"
#include "pbd2_collision_object.h"

#include "core/print_string.h"

namespace Pbd
{

static void clear_contacts( Simulation * sim )
{
    sim->contacts_all.clear();
    sim->contacts_all_qtys.clear();
}

static void store_contacts( Simulation * sim, const Vector<ContactPointBb> & contacts )
{
    const int qty = contacts.size();
    for ( int i=0; i<qty; i++ )
    {
        const ContactPointBb & pt = contacts.ptr()[i];
        sim->contacts_all.push_back( pt );
    }
    sim->contacts_all_qtys.push_back( qty );
}

Simulation::Simulation()
{
    h = 0.01;
    solver_iterations = 3;
    time_remainder    = 0.0;
	contact_erp       = 1.0;

    step_number = 0;
}

Simulation::~Simulation()
{
}

void Simulation::set_broad_phase_max_depth( int depth )
{
	tree.set_max_depth( depth );
}

int  Simulation::get_broad_phase_max_depth() const
{
	return tree.max_depth();
}
    
void Simulation::set_time_step( Float h )
{
    this->h = h;
}

Float Simulation::get_time_step() const
{
	return h;
}

void Simulation::set_solver_iterations( int qty )
{
    solver_iterations = qty;
}

int Simulation::get_solver_iterations() const
{
	return solver_iterations;
}

void Simulation::set_contact_erp( Float k )
{
	if ( k < 0.0 )
		k = 0.0;
	else if ( k > 1.0 )
		k = 1.0;
	contact_erp = k;
}

Float Simulation::get_contact_erp() const
{
	return contact_erp;
}

void Simulation::step( Float delta )
{
    Float t = time_remainder + delta;
    while ( t > 0.0 )
    {
        step();
        t -= h;
    }
    time_remainder = t;
}

void Simulation::step()
{
    // Clear all contacts from previous simulation step.
    clear_contacts( this );
	// Update joint lists.
	//update_joint_lists( this );

    // Integrate dynamics.
    const int bodies_qty = bodies.size();
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        body->integrate_dynamics( h );
    }

    // Initialize joint lambdas.
    const int joints_qty = joints.size();
    for ( int i=0; i<joints_qty; i++ )
    {
        Joint * joint = joints.ptr()[i];
        joint->init_lambdas();
    }
    
    // Initialize contact lambdas.
    for ( int i=0; i<bodies_qty; i++ )
    {
        // Here contact point solver lambdas init should go.
        RigidBody * body = bodies.ptr()[i];
        body->init_contact_lambdas();
    }

    // Solve joint and contact constraints.
    for ( int i=0; i<solver_iterations; i++ )
    {
        for ( int j=0; j<joints_qty; j++ )
        {
            Joint * joint = joints.ptr()[j];
            joint->solver_step( h );
        }
    }

    // Solve for normal collisions body with another body.
    tree.subdivide( this, h );
	for ( int i=0; i<bodies_qty; i++ )
	{
		RigidBody * body_a = bodies.ptrw()[i];
		// Collide with other bodies.
		// List of potentially colliding pairs.
		// Not using contact reference here because points are to be modified modified (lambdas are modified during interations).
		// Returned contacts are for a single pair of objects.
		contacts = tree.find_contact_points( body_a, h );
		const int contacts_qty = contacts.size();
		for ( int j=0; j<contacts_qty; j++ )
		{
			ContactPointBb & pt = contacts.ptrw()[j];
			RigidBody * body_a = pt.body_a;
			RigidBody * body_b = pt.body_b;
			body_a->orig_vel = body_a->vel;
			body_b->orig_vel = body_b->vel;
		}
		solve_simple_all( contacts, contact_erp, h );
		const int qty = contacts.size();
		for ( int j=0; j<contacts_qty; j++ )
		{
			const ContactPointBb & pt = contacts.ptr()[j];
			contacts_all.push_back( pt );
		}
	}

    // Update velocities.
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        body->update_velocities( h );
    }
	solve_dynamic_friction_simple_all( contacts_all, h );

    step_number += 1;
}

void Simulation::clear()
{
    bodies.clear();
    joints.clear();
}

void Simulation::add_body( RigidBody * body )
{
    bodies.push_back( body );
}

void Simulation::add_joint( Joint * joint )
{
    joints.push_back( joint );
}

void Simulation::update_joint_lists( Simulation * sim )
{
	const int bodies_qty = sim->bodies.size();
	const int joints_qty = sim->joints.size();
	for ( int i=0; i<bodies_qty; i++ )
	{
		RigidBody * body = sim->bodies.ptrw()[i];
		body->joints.clear();
	}

	for ( int i=0; i<joints_qty; i++ )
	{
		Joint * joint = sim->joints.ptrw()[i];
		RigidBody * body_a = joint->body_a;
		if ( body_a != nullptr )
			body_a->joints.push_back( joint );
		RigidBody * body_b = joint->body_b;
		if ( body_b != nullptr )
			body_b->joints.push_back( joint );
	}
}

bool Simulation::solve_normal( RigidBody * body_a, RigidBody * body_b, Vector<ContactPointBb> & pts, Float h )
{
    Float w_a;
    const bool ok_a = specific_mass_pos( true, body_a, pts, w_a );
    if ( !ok_a )
        return false;

    Float w_b;
    const bool ok_b = specific_mass_pos( false, body_b, pts, w_b );
    if ( !ok_b )
        return false;

    const Float w_both = w_a + w_b;

    const Float compliance_a = body_a->compliance_normal;
    const Float compliance_b = body_b->compliance_normal;
    const Float compliance_normal = (compliance_a + compliance_b) / 2.0;

    Vector3d d_accum       = Vector3d::ZERO;
    Vector3d r_x_d_accum_a = Vector3d::ZERO;
    Vector3d r_x_d_accum_b = Vector3d::ZERO;
 
    const int qty = pts.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPointBb & cp = pts.ptrw()[i];

        // Compute lambda_normal. It is needed for further tangential procesing.
        Float lambda = cp.lambda_normal;

        const Float alpha_ = compliance_normal / (h*h);
        const Float d_lambda = (cp.depth + alpha_*lambda) / (w_both + alpha_);
        lambda += d_lambda;
        cp.lambda_normal = lambda;

        const Vector3d d = d_lambda * cp.n_world;
        d_accum += d;

        const Vector3d r_w_a = body_a->pose.q * cp.r_a;
        const Vector3d r_x_d_a = r_w_a.CrossProduct( d );
        r_x_d_accum_a += r_x_d_a;

        const Vector3d r_w_b = body_b->pose.q * cp.r_b;
        const Vector3d r_x_d_b = r_w_b.CrossProduct( d );
        r_x_d_accum_b += r_x_d_b;
    }

    // Body "a".
    // Plus for body "a".
    if ( body_a->mass > 0.0 )
    {
        const Vector3d dr = d_accum / body_a->mass;
        const Matrix3d inv_I = body_a->inv_I();
        const Vector3d rot = inv_I * r_x_d_accum_a * 0.5;
        Quaterniond dq( 0.0, rot.x_, rot.y_, rot.z_ );
        const Pose pose = body_a->pose;
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
        const Vector3d dr = d_accum / body_b->mass;
        const Matrix3d inv_I = body_b->inv_I();
        const Vector3d rot = inv_I * r_x_d_accum_b * 0.5;
        Quaterniond dq( 0.0, rot.x_, rot.y_, rot.z_ );
        const Pose pose = body_b->pose;
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

    return true;
}

void Simulation::solve_tangential( RigidBody * body_a, RigidBody * body_b, Vector<ContactPointBb> & pts, Float h )
{
    const int qty = pts.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPointBb & pt = pts.ptrw()[i];
        pt.solve_tangential( body_a, body_b, h );
    }
}

void Simulation::solve_dynamic_friction( Simulation * sim, int base_ind, int qty, Float h )
{
    for ( int i=0; i<qty; i++ )
    {
        const int ind = base_ind + i;
        ContactPointBb & pt = sim->contacts_all.ptrw()[ind];
        RigidBody * body_a = pt.body_a;
        RigidBody * body_b = pt.body_b;
        pt.solve_dynamic_friction( body_a, body_b, h );
    }
}


bool Simulation::specific_mass_pos( bool is_a, RigidBody * body, const Vector<ContactPointBb> & pts, Float & w )
{
    w = 0.0;
    if ( body->mass <= 0.0 )
        return true;

    Vector3d r_x_n_accum = Vector3d( 0.0, 0.0, 0.0 );
    const int qty = pts.size();
    if ( qty < 1 )
        return false;
    
    for ( int i=0; i<qty; i++ )
    {
        const ContactPointBb & pt = pts.ptr()[i];
        const Vector3d & rl = (is_a) ? pt.r_a : pt.r_b;
        const Vector3d rw = body->pose.q * rl;
        const Vector3d r_x_n = rw.CrossProduct( pt.n_world );
        r_x_n_accum += r_x_n;
    }
    const Matrix3d inv_I = body->inv_I();
    const Float position_part = static_cast<Float>(qty)/body->mass;
    const Float rotation_part = r_x_n_accum.DotProduct( inv_I * r_x_n_accum );
    w = position_part + rotation_part;
    
    return true;
}

void Simulation::store_body_states( Vector<RigidBody *> & bodies )
{
	const int bodies_qty = bodies.size();
	for ( int i=0; i<bodies_qty; i++ )
	{
		RigidBody * body = bodies.ptrw()[i];
		body->orig_vel   = body->vel;
	}
}

void Simulation::solve_simple_all( Vector<ContactPointBb> & pts, Float contact_erp, Float h )
{
	const int qty = pts.size();
	for ( int i=0; i<qty; i++ )
	{
		ContactPointBb & pt = pts.ptrw()[i];
		RigidBody * body_a = pt.body_a;
		RigidBody * body_b = pt.body_b;
		pt.solve_normal( body_a, body_b, contact_erp, h );
		pt.solve_tangential( body_a, body_b, h );
	}
}

void Simulation::solve_dynamic_friction_simple_all( Vector<ContactPointBb> & pts, Float h )
{
	const int qty = pts.size();
	for ( int i=0; i<qty; i++ )
	{
		ContactPointBb & pt = pts.ptrw()[i];
		RigidBody * body_a = pt.body_a;
		RigidBody * body_b = pt.body_b;
		pt.solve_dynamic_friction( body_a, body_b, h );
	}
}


}







