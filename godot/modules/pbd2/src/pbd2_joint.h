
#ifndef __PBD2_JOINT_H_
#define __PBD2_JOINT_H_

#include "vector3d.h"

using namespace Ign;

namespace Pbd
{

class RigidBody;

class Joint
{
public:
    Joint();
    virtual ~Joint();

	void set_ignore_collisions( bool en );
	bool get_ignore_collisions() const;

    // Places as many zeros to "lambdas" array as needed.
    virtual void init_lambdas() {}
    // This thing should be implemented. It is supposed to set 
    // motor target to the current relative configuration to 
    // avoid initial "jump".
    virtual void init_motor_target() {}
     // performs all necessary steps. Each with its own lambda.
    virtual void solver_step( Float h ) {}
   
    // Orthogonalizes joint basis vectors 
    // "e1_a", "e2_a", "e3_a" and "e1_b", "e2_b", "e3_b".
    void orthogonalize();

	// Makes at_a and at_b to be in one point.
	// It computes the miggle point of a common perpendicular.
	void init_joint_at_vectors();

    RigidBody * body_a;
    RigidBody * body_b;

    // Force and dtrque applied to bodies.
    // These are outputs.
    Vector3d force, torque;

    Float compliance_joint;
    Float compliance_motor;
    bool  motor;
	bool ignore_collisions;

    Vector3d at_a;
    Vector3d e1_a;
    Vector3d e2_a;
    Vector3d e3_a;

    Vector3d at_b;
    Vector3d e1_b;
    Vector3d e2_b;
    Vector3d e3_b;

    Vector<Float> lambdas;

    static const Float EPS;
    static const Float _2_PI;
};



}



#endif





