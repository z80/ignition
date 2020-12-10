
#ifndef __PBD2_JOINT_NODE_H_
#define __PBD2_JOINT_NODE_H_

#include "scene/main/node.h"
#include "pbd2_joint.h"

using namespace Ign;

namespace Pbd
{

class Joint;

class JointNode: public Node
{
    GDCLASS( JointNode, Node );

public:
    JointNode();
     ~JointNode();

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






};



}



#endif






