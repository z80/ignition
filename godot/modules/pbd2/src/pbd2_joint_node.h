
#ifndef __PBD2_JOINT_NODE_H_
#define __PBD2_JOINT_NODE_H_

#include "scene/main/node.h"
#include "pbd2_joint.h"

using namespace Ign;

namespace Pbd
{

class Joint;

class PbdJointNode: public Node
{
    GDCLASS( PbdJointNode, Node );

public:
    PbdJointNode();
     virtual ~PbdJointNode();

    void set_body_path_a( const NodePath & p );
    NodePath get_body_path_a() const;

    void set_body_path_b( const NodePath & p );
    NodePath get_body_path_b() const;

    void set_at_a( const Vector3 & at );
    Vector3 get_at_a() const;

    void set_at_b( const Vector3 & at );
    Vector3 get_at_b() const;

    void set_e1_a( const Vector3 & e );
    Vector3 get_e1_a() const;

    void set_e2_a( const Vector3 & e );
    Vector3 get_e2_a() const;

    void set_e3_a( const Vector3 & e );
    Vector3 get_e3_a() const;

    void set_e1_b( const Vector3 & e );
    Vector3 get_e1_b() const;

    void set_e2_b( const Vector3 & e );
    Vector3 get_e2_b() const;

    void set_e3_b( const Vector3 & e );
    Vector3 get_e3_b() const;

    void set_motor( bool en );
    bool get_motor() const;

    void set_compliance_joint( real_t k );
    real_t get_compliance_joint() const;

    void set_compliance_motor( real_t k );
    real_t get_compliance_motor() const;

    Vector3 get_force() const;
    Vector3 get_torque() const;

protected:
    void _notifications( int p_what );
    static void _bind_methods();

private:
    RigidBody * body_from_path( const NodePath & path );

public: 
    // In derived class assign this pointer to 
    // point to an instance of an actual joint.
    Joint * joint;
    NodePath body_a;
    NodePath body_b;

};



}



#endif







