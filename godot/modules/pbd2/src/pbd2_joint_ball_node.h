
#ifndef __PBD2_JOINT_BALL_NODE_H_
#define __PBD2_JOINT_BALL_NODE_H_

#include "pbd2_joint_node.h"
#include "pbd2_joint_ball.h"


namespace Pbd
{

class PbdJointBallNode: public PbdJointNode
{
    GDCLASS( PbdJointBallNode, PbdJointNode );

public:
    PbdJointBallNode();
    ~PbdJointBallNode();

    void set_spatial_gap( real_t r );
    real_t get_spatial_gap() const;

    void set_angular_gap( real_t a );
    real_t get_angular_gap() const;

    void set_motor_gap( real_t r );
    real_t get_motor_gap() const;

    void set_target_position( const Quat & q );
    Quat get_target_position() const;

	void set_target_velocity( const Vector3 & w );
	Vector3 get_target_velocity() const;

	void set_position_control( bool en );
	bool get_position_control() const;

protected:
    static void _bind_methods();

    JointBall joint_ball;
};


}




#endif


