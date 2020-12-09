
#ifndef __PBD2_JOINT_HINGE_H_
#define __PBD2_JOINT_HINGE_H_

#include "pbd2_joint.h"

namespace Pbd
{

class JointHinge: public Joint
{
public:
    JointHinge();
    ~JointHinge();

    void init_lambdas();
    void init_motor_target();
    void solver_step( Float h );

    Float solver_step_position( Float lambda, Float h );
    Float solver_step_rotation( Float lambda, Float h );
    Float solver_step_motor( Float lambda, Float h );

    Vector3d _delta_r() const;
    Float _delta_theta() const;
    Vector3d _world_r( RigidBody * body, const Vector3d & r ) const;
    Float _delta_motor() const;

    // Defining the looseness of the joint.
    // Bodies are not connected perfectly.
    // There can be gaps like in a chain between the links.
    // Those are define by spatial gap and angular gap.
    Float spatial_gap;
    Float angular_gap;

    // Defining the motor.
    Float target_position;
    Float target_velocity;
    Float motor_gap;
    bool position_control;
};



}




#endif





