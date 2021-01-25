
#include "pbd2_simulation.h"
#include "pbd2_rigid_body.h"
#include "pbd2_joint.h"
#include "pbd2_collision_object.h"

namespace Pbd
{

static void clear_contacts( RigidBody * body )
{
    CollisionObject * co = body->collision_objects.ptrw()[0];
    if ( co == nullptr )
        return;
    co->contacts.clear();
}

static void store_contacts( RigidBody * body, const Vector<ContactPointBb> & contacts )
{
    CollisionObject * co = body->collision_objects.ptrw()[0];
    if ( co == nullptr )
        return;
    const int qty = contacts.size();
    for ( int i=0; i<qty; i++ )
    {
        const ContactPointBb & pt = contacts.ptr()[i];
        co->contacts.push_back( pt );
    }
}

Simulation::Simulation()
{
    h = 0.01;
    solver_iterations = 3;
    time_remainder = 0.0;
}

Simulation::~Simulation()
{
}
    
void Simulation::set_time_step( Float h )
{
    this->h = h;
}

void Simulation::set_solver_iterations( int qty )
{
    solver_iterations = qty;
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
    // Integrate dynamics.
    const int bodies_qty = bodies.size();
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
		clear_contacts( body );
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

        //for ( int j=0; j<bodies_qty; j++ )
        //{
        //    RigidBody * body = bodies.ptr()[j];
        //    //body->solve_contacts( h );
        //    body->solve_normal_all( h );
        //    body->solve_tangential_all( h );
        //}
    }

    // Solve for normal collisions body with another body.
    tree.subdivide( this, h );
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body_a = bodies.ptrw()[i];
        // Collide with other bodies.
        // List of potentially colliding pairs.
        RigidBody * body_b;
        // Not using contact reference here because points are to be modified modified (lambdas are modified during interations).
        contacts = tree.contact_points( body_a, h, body_b );
        const int contacts_qty = contacts.size();
		if ( contacts_qty > 0 )
		{
            for ( int j=0; j<solver_iterations; j++ )
            {
                solve_normal( body_a, body_b, contacts, h );
                solve_tangential( body_a, body_b, contacts, h );
            }
            // Store contacts for future use in "solve_dynamic_friction".
            // Storing after as lambdas are modified.
            store_contacts( body_a, contacts );
		}
    }

    // Update velocities.
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        body->update_velocities( h );
    }

    // Update contact velocities.
    for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        //body->solve_dynamic_friction( h );
        // The same for ContactPointBb. Those are stored inside body->collision_object.
        solve_dynamic_friction( body, h );
    }

    // Update contact prev. positions.
    /*for ( int i=0; i<bodies_qty; i++ )
    {
        RigidBody * body = bodies.ptr()[i];
        body->update_contact_positions();
    }*/
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

    Vector3d d_accum     = Vector3d::ZERO;
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
		body_a->orig_pose = body_a->pose;
		body_a->orig_vel  = body_a->vel;

        const Vector3d dr = d_accum / body_a->mass;
        const Matrix3d inv_I = body_a->inv_I();
        const Vector3d rot = inv_I * r_x_d_accum_a;
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
		body_b->orig_pose = body_b->pose;
		body_b->orig_vel  = body_b->orig_vel;

        const Vector3d dr = d_accum / body_b->mass;
        const Matrix3d inv_I = body_b->inv_I();
        const Vector3d rot = inv_I * r_x_d_accum_b;
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

bool Simulation::solve_tangential( RigidBody * body_a, RigidBody * body_b, Vector<ContactPointBb> & pts, Float h )
{
    const int qty = pts.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPointBb & pt = pts.ptrw()[i];
        pt.solve_tangential( body_a, body_b, h );
    }
    return true;
}

bool Simulation::solve_dynamic_friction( RigidBody * body, Float h )
{
    CollisionObject * co = body->collision_objects.ptrw()[0];
    if ( co == nullptr )
        return false;
    Vector<ContactPointBb> & contacts = co->contacts;
    const int qty = contacts.size();
    for ( int i=0; i<qty; i++ )
    {
        ContactPointBb & pt = contacts.ptrw()[i];
		RigidBody * body_a = body;
		RigidBody * body_b = pt.body_b;
        pt.solve_dynamic_friction( body_a, body_b, h );
    }

    return true;
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



}







