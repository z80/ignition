
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

    // Places as many zeros to "lambdas" array as needed.
    virtual void init_lambdas() = 0;
    // This thing should be implemented. It is supposed to set 
    // motor target to the current relative configuration to 
    // avoid initial "jump".
    virtual void init_motor_target() = 0;
     // performs all necessary steps. Each with its own lambda.
    virtual void solver_step( Float h ) = 0;
   
    // Orthogonalizes joint basis vectors 
    // "e1_a", "e2_a", "e3_a" and "e1_b", "e2_b", "e3_b".
    void orthogonalize();


    RigidBody * body_a;
    RigidBody * body_b;

    // Force and dtrque applied to bodies.
    // These are outputs.
    Vector3d force, torque;

    Float compliance_joint;
    Float compliance_motor;
    bool  motor;

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






