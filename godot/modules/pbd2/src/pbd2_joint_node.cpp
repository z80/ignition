
#include "pbd2_joint_node.h"
#include "pbd2_simulation_node.h"
#include "pbd2_rigid_body_node.h"
#include "pbd2_joint.h"

namespace Pbd
{

static void enter_tree( PbdJointNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulationNode>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.joints.push_back( rbn->joint );
}

static void exit_tree( PbdJointNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulationNode>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.joints.erase( rbn->joint );
}



PbdJointNode::PbdJointNode()
    : Node()
{
    joint = nullptr;
}

PbdJointNode::~PbdJointNode()
{
}

void PbdJointNode::set_body_path_a( const NodePath & p )
{
    body_a = p;
    if ( joint != nullptr )
        joint->body_a = body_from_path( body_a );
}

NodePath PbdJointNode::get_body_path_a() const
{
    return body_a;
}

void PbdJointNode::set_body_path_b( const NodePath & p )
{
    body_b = p;
    if ( joint != nullptr )
        joint->body_b = body_from_path( body_b );
}

NodePath PbdJointNode::get_body_path_b() const
{
    return body_b;
}

void PbdJointNode::set_at_a( const Vector3 & at )
{
    if ( joint != nullptr )
        joint->at_a = Vector3d( at.x, at.y, at.z );
}

Vector3 PbdJointNode::get_at_a() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->at_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_at_b( const Vector3 & at )
{
    if ( joint != nullptr )
        joint->at_b = Vector3d( at.x, at.y, at.z );
}

Vector3 PbdJointNode::get_at_b() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->at_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_e1_a( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e1_a = Vector3d( e.x, e.y, e.z );
}

Vector3 PbdJointNode::get_e1_a() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->e1_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_e2_a( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e2_a = Vector3d( e.x, e.y, e.z );
}

Vector3 PbdJointNode::get_e2_a() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->e2_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_e3_a( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e3_a = Vector3d( e.x, e.y, e.z );
}

Vector3 PbdJointNode::get_e3_a() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->e2_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_e1_b( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e1_b = Vector3d( e.x, e.y, e.z );
}

Vector3 PbdJointNode::get_e1_b() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->e1_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_e2_b( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e2_b = Vector3d( e.x, e.y, e.z );
}

Vector3 PbdJointNode::get_e2_b() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->e2_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_e3_b( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e3_b = Vector3d( e.x, e.y, e.z );
}

Vector3 PbdJointNode::get_e3_b() const
{
    if ( joint == nullptr )
        return Vector3();
    
    const Vector3d & a = joint->e3_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void PbdJointNode::set_motor( bool en )
{
    if ( joint != nullptr )
        joint->motor = en;
}

bool PbdJointNode::get_motor() const
{
    if ( joint == nullptr )
        return false;
    return joint->motor;
}

void PbdJointNode::set_compliance_joint( real_t k )
{
    if ( joint != nullptr )
        joint->compliance_joint = k;
}

real_t PbdJointNode::get_compliance_joint() const
{
    if ( joint == nullptr )
        return 0.0;
    return joint->compliance_joint;
}

void PbdJointNode::set_compliance_motor( real_t k )
{
    if ( joint != nullptr )
        joint->compliance_motor = k;
}

real_t PbdJointNode::get_compliance_motor() const
{
    if ( joint == nullptr )
        return 0.0;
    return joint->compliance_motor;
}

Vector3 PbdJointNode::get_force() const
{
    if ( joint == nullptr )
        return Vector3();

    const Vector3d & F = joint->force;
    const Vector3 f( F.x_, F.y_, F.z_ );
    return f;
}

Vector3 PbdJointNode::get_torque() const
{
    if ( joint == nullptr )
        return Vector3();

    const Vector3d & P = joint->torque;
    const Vector3 p( P.x_, P.y_, P.z_ );
    return p;
}



void PbdJointNode::_notifications( int p_what )
{
    switch ( p_what )
    {
        case NOTIFICATION_READY:
            if ( joint != nullptr )
            {
                joint->body_a = body_from_path( body_a );
                joint->body_b = body_from_path( body_b );
            }
            break;

        case NOTIFICATION_ENTER_TREE:
            enter_tree( this );
            break;

        case NOTIFICATION_EXIT_TREE:
            exit_tree( this );

        default:
            break;
    }
}

void PbdJointNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_body_path_a", "path" ), &PbdJointNode::set_body_path_a );
    ClassDB::bind_method( D_METHOD( "get_body_path_a" ),         &PbdJointNode::get_body_path_a, Variant::NODE_PATH );

    ClassDB::bind_method( D_METHOD( "set_body_path_b", "path" ), &PbdJointNode::set_body_path_b );
    ClassDB::bind_method( D_METHOD( "get_body_path_b" ),         &PbdJointNode::get_body_path_b, Variant::NODE_PATH );

    ClassDB::bind_method( D_METHOD( "set_at_a", "at" ), &PbdJointNode::set_at_a );
    ClassDB::bind_method( D_METHOD( "get_at_a" ),       &PbdJointNode::get_at_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_at_b", "at" ), &PbdJointNode::set_at_b );
    ClassDB::bind_method( D_METHOD( "get_at_b" ),       &PbdJointNode::get_at_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e1_a", "at" ), &PbdJointNode::set_e1_a );
    ClassDB::bind_method( D_METHOD( "get_e1_a" ),       &PbdJointNode::get_e1_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e2_a", "at" ), &PbdJointNode::set_e2_a );
    ClassDB::bind_method( D_METHOD( "get_e2_a" ),       &PbdJointNode::get_e2_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e3_a", "at" ), &PbdJointNode::set_e3_a );
    ClassDB::bind_method( D_METHOD( "get_e3_a" ),       &PbdJointNode::get_e3_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e1_b", "at" ), &PbdJointNode::set_e1_b );
    ClassDB::bind_method( D_METHOD( "get_e1_b" ),       &PbdJointNode::get_e1_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e2_b", "at" ), &PbdJointNode::set_e2_b );
    ClassDB::bind_method( D_METHOD( "get_e2_b" ),       &PbdJointNode::get_e2_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e3_b", "at" ), &PbdJointNode::set_e3_b );
    ClassDB::bind_method( D_METHOD( "get_e3_b" ),       &PbdJointNode::get_e3_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_motor", "en" ), &PbdJointNode::set_motor );
    ClassDB::bind_method( D_METHOD( "get_motor" ),       &PbdJointNode::get_motor, Variant::BOOL );

    ClassDB::bind_method( D_METHOD( "set_compliance_joint", "k" ), &PbdJointNode::set_compliance_joint );
    ClassDB::bind_method( D_METHOD( "get_compliance_joint" ),      &PbdJointNode::get_compliance_joint, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_compliance_motor", "k" ), &PbdJointNode::set_compliance_motor );
    ClassDB::bind_method( D_METHOD( "get_compliance_motor" ),      &PbdJointNode::get_compliance_motor, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "get_force" ),  &PbdJointNode::get_force, Variant::VECTOR3 );
    ClassDB::bind_method( D_METHOD( "get_torque" ), &PbdJointNode::get_torque, Variant::VECTOR3 );


    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "body_path_a" ), "set_body_path_a", "get_body_path_a" );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "body_path_b" ), "set_body_path_b", "get_body_path_b" );

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "at_a" ),        "set_at_a", "get_at_a" );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "at_b" ),        "set_at_b", "get_at_b" );

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e1_a" ),        "set_e1_a", "get_e1_a" );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e2_a" ),        "set_e2_a", "get_e2_a" );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e3_a" ),        "set_e3_a", "get_e3_a" );

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e1_b" ),        "set_e1_b", "get_e1_b" );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e2_b" ),        "set_e2_b", "get_e2_b" );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e3_b" ),        "set_e3_b", "get_e3_b" );

    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "motor" ),            "set_motor", "get_motor" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "compliance_joint" ), "set_compliance_joint", "get_compliance_joint" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "compliance_motor" ), "set_compliance_motor", "get_compliance_motor" );
}

RigidBody * PbdJointNode::body_from_path( const NodePath & path )
{
    Node * n = get_node( path );
    if ( n == nullptr )
        return nullptr;
    PbdRigidBodyNode * rbn = Node::cast_to<PbdRigidBodyNode>( n );
    if ( rbn == nullptr )
        return nullptr;

    return &(rbn->rigid_body);
}
 


}

