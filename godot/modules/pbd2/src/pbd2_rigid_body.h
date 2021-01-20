
#ifndef __PBD2_RIGID_BODY_H_
#define __PBD2_RIGID_BODY_H_

#include "pbd2_pose.h"
#include "matrix3d.h"
#include "pbd2_contact_point.h"

using namespace Ign;

namespace Pbd
{

class CollisionObject;

class RigidBody
{
public:
    Pose pose;
    Pose prev_pose;
    Pose orig_pose;
    Vector3d vel;
    Vector3d omega;

    Float mass;
    Matrix3d inertia;
    Matrix3d inv_inertia;

    // Force and torque applied.
    Vector3d force, torque;

    Float friction;
    Float damping_linear;
    Float damping_angular;
    Float restitution;

    Float compliance_normal;
    Float compliance_tangential;

    // These are fixed points in body's ref frame.
    // It is checked if a point is below "y=0" plane.
    Vector<ContactPoint> contact_points;

    // Collision object for colliding mobing bodies.
    CollisionObject * collision_object;

    RigidBody();
    ~RigidBody();
    RigidBody( const RigidBody & inst );
    const RigidBody & operator=( const RigidBody & inst );

    void set_inertia( const Matrix3d & I );
    Matrix3d I() const;
    Matrix3d inv_I() const;

    void integrate_dynamics( Float dt );
    void update_velocities( Float dt );
    void init_contact_lambdas();
    void solve_contacts( Float h );
    void update_contact_velocities( Float h );
    void update_contact_positions();

    Float specific_inv_mass_pos( const Vector3d & r, const Vector3d & n ) const;
    Float specific_inv_mass_rot( const Vector3d & n ) const;
    Float specific_inv_mass_pos_all( bool check_in_contact );

    bool solve_normal_all( Float h );
    void solve_tangential_all( Float h );
    void solve_dynamic_friction( Float h );
};



}



#endif




