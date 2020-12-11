
#ifndef __PBD2_RIGID_BODY_NODE_H_
#define __PBD2_RIGID_BODY_NODE_H_

#include "scene/3d/spatial.h"
#include "pbd2_rigid_body.h"

namespace Pbd
{

class PbdRigidBodyNode: public Spatial
{
    GDCLASS( PbdRigidBodyNode, Spatial );

public:
    PbdRigidBodyNode();
    ~PbdRigidBodyNode();

    void set_mass( real_t m );
    real_t get_mass() const;

    void set_inertia( const Basis & I );
    Basis get_inertia() const;

    void set_transform_rb( const Transform & t );
    Transform get_transform_rb() const;

    void set_linear_velocity( const Vector3 & v );
    Vector3 get_linear_velocity() const;

    void set_angular_velocity( const Vector3 & w );
    Vector3 get_angular_velocity() const;

    void set_friction( real_t k );
    real_t get_friction() const;

    void set_restitution( real_t k );
    real_t get_restitution() const;

    void set_force( const Vector3 & f );
    Vector3 get_force() const;

    void set_torque( const Vector3 & p );
    Vector3 get_torque() const;

    void rebuild_contacts();

protected:
    void _notifications( int p_what );
    static void _bind_methods();

public:
    RigidBody rigid_body;
};


}


#endif





