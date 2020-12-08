
#ifndef __PBD2_JOINT_BALL_H_
#define __PBD2_JOINT_BALL_H_

#include "pbd2_joint.h"


namsespace Pbd
{

class JointBall: public Joint
{
public:
    JointBall();
    ~JointBall();

    void init_lambdas();
    void init_motor_target();
    void solver_step( Float h );

    Float solver_step_position( Float lambda, Float h );
    Float solver_step_motor( Float lambda, Float h );

    Vector3d _delta_r() const;
    Vector3d _world_r( RigidBody * body, const Vector3d & r ) const;
    Float _delta_motor_eta() const;
    Float _delta_motor_theta() const;
    Float _delta_motor_zeta() const;

    Float spatial_gap;
    Float angular_gap;
    Float motor_gap;

    Quaterniond target_q;

};


}


#endif





