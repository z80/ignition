
#ifndef __PBD2_RIGID_BODY_H_
#define __PBD2_RIGID_BODY_H_

#include "pbd2_pose.h"
#include "matrix3d.h"
#include "pbd2_contact_point.h"

using namespace Ign;

namespace Pbd
{

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

    Vector<Vector3d> contact_points;

    RigidBody();
    ~RigidBody();
    RigidBody( const RigidBody & inst );
    const RigidBody & operator=( const RigidBody & inst );

    void set_inertia( const Matrix3d & I );
    Matrix33 I() const;
    Matrix33 inv_I() const;

    void integrate_dynamics( Float dt );
    void update_velocities( Float dt );
    void init_contact_lambdas();
    void solve_contacts( Float h );
    void update_contact_velocities( Float h );

    Float specific_inv_mass_pos( const Vector3d & r, const Vector3d & n );
    Float specific_inv_mass_rot( const Vector3d & n );
};



}



#endif




