
#ifndef __PBD2_CONTACT_POINT_H_
#define __PBD2_CONTACT_POINT_H_

#include "vector3d.h"

using namespace Ign;

namespace Pbd
{

class RigidBody;

class ContactPoint
{
public:
    ContactPoint();
    ~ContactPoint();
    ContactPoint( const ContactPoint & inst );
    const ContactPoint & operator=( const ContactPoint & inst );
    
    void set_in_contact( bool en );
    void init_lambdas();
    void solve( RigidBody * body, Float h );
    void solve_dynamic_friction( RigidBody * body, Float h );

    bool solve_normal( RigidBody * body, Float h );
    void solve_tangential( RigidBody * body, Float h );

    // Point position in body ref. frame.
    Vector3d r;
    Vector3d r_world;
    Vector3d r_world_prev;
    Vector3d v_world;
    Vector3d n_world;

    Float lambda_normal;
    Float lambda_tangential;
    
    bool in_contact;

    static const Float EPS;
};

}




#endif





