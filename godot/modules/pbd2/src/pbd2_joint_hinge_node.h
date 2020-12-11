
#ifndef __PBD2_JOINT_HINGE_NODE_H_
#define __PBD2_JOINT_HINGE_NODE_H_

#include "pbd2_joint_node.h"
#include "pbd2_joint_hinge.h"

namespace Pbd
{

class PbdJointHingeNode: public PbdJointNode
{
    GDCLASS( PbdJointHingeNode, PbdJointNode );

public:
    PbdJointHingeNode();
    ~PbdJointHingeNode();

    void set_spatial_gap( real_t r );
    real_t get_spatial_gap() const;

    void set_angular_gap( real_t r );
    real_t get_angular_gap() const;

    void set_target_position( real_t r );
    real_t get_target_position() const;

    void set_target_velocity( real_t v );
    real_t get_target_velocity() const;

    void set_motor_gap( real_t r );
    real_t get_motor_gap() const;

    void set_position_control( bool en );
    bool get_position_control() const;

protected:
    static void _bind_methods();

    JointHinge joint_hinge;
};



}



#endif



