
#include "pbd2_joint.h"

namespace Pbd
{

static void orthogonalize( Vector3d & e1, Vector3d & e2, Vector3d & e3 );

const Float Joint::EPS   = 0.0001;
const Float Joint::_2_PI = 6.283185307179586;


Joint::Joint()
{
    at_a = Vector3d( 0.0,  1.0, 0.0 );
    at_b = Vector3d( 0.0, -1.0, 0.0 );

    e1_a = Vector3d( 1.0, 0.0, 0.0 );
    e2_a = Vector3d( 0.0, 1.0, 0.0 );
    e3_a = Vector3d( 0.0, 0.0, 1.0 );

    e1_b = Vector3d( 1.0, 0.0, 0.0 );
    e2_b = Vector3d( 0.0, 1.0, 0.0 );
    e3_b = Vector3d( 0.0, 0.0, 1.0 );

	compliance_joint = 0.0;
	compliance_motor = 0.0;
	motor            = false;

    body_a = nullptr;
    body_b = nullptr;
}

Joint::~Joint()
{
}

void Joint::orthogonalize()
{
    Pbd::orthogonalize( e1_a, e2_a, e3_a );
    Pbd::orthogonalize( e1_b, e2_b, e3_b );
}

void Joint::init_joint_at_vectors()
{
	// TODO
	// Also, may be make it optional.
	// ......
}


static void orthogonalize( Vector3d & e1, Vector3d & e2, Vector3d & e3 )
{
    const Vector3d u1 = e1;
    
    Vector3d u2 = e2 - ( u1*e2.ProjectOntoAxis(u1) );
    u2.Normalize();

    Vector3d u3 = e3 - ( u1*e3.ProjectOntoAxis(u1) ) - ( u2*e3.ProjectOntoAxis(u2) );
    u3.Normalize();

    e1 = u1;
    e2 = u2;
    e3 = u3;
}


}

