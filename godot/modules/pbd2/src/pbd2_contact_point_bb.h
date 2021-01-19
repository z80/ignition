
#ifndef __PBD2_CONTACT_POINT_BB_H_
#define __PBD2_CONTACT_POINT_BB_H_

#include "vector3d.h"

using namespace Ign;

namespace Pbd
{

class RigidBody;

class ContactPointBb
{
public:
    ContactPointBb();
    ~ContactPointBb();
    ContactPointBb( const ContactPointBb & inst );
    const ContactPointBb & operator=( const ContactPointBb & inst );
    
    void init_lambdas();
    void solve_tangential( RigidBody * body_a, RigidBody * body_b, Float h );
    void solve_dynamic_friction( RigidBody * body_a, RigidBody * body_b, Float h );

    // Point position in body ref. frame.
    // Positions in local body ref. frames.
    Vector3d r_a, r_b;
    // Normal in world ref frame. IMPORTANT: it is towards body "a" (!!!)
    Vector3d n_world;
    // Penetration depth.
    Float    depth;

    Float lambda_normal;
    Float lambda_tangential;
    
    bool apply_friction;

    static const Float EPS;
};

}




#endif





