
#include "pbd2_joint.h"

namespace Pbd
{

static void enter_tree( PbdJointNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulation>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.joints.push_back( rbn->joint );
}

static void exit_tree( PbdJointNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulation>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.joints.erase( rbn->joint );
}



JointNode::JointNode()
    : Node()
{
    joint = nullptr;
}

JointNode::~JointNode()
{
}

void JointNode::set_body_path_a( const NodePath & p )
{
    body_a = p;
    if ( joint != nullptr )
        joint->body_a = body_from_path( body_a );
}

NodePath JointNode::get_body_path_a() const
{
    return body_a;
}

void JointNode::set_body_path_b( const NodePath & p )
{
    body_b = p;
    if ( joint != nullptr )
        joint->body_b = body_from_path( body_b );
}

NodePath JointNode::get_body_path_b() const
{
    return body_b;
}

void JointNode::set_at_a( const Vector3 & at )
{
    if ( joint != nullptr )
        joint->at_a = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_at_a() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->at_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_at_b( const Vector3 & at )
{
    if ( joint != nullptr )
        joint->at_b = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_at_b() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->at_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_e1_a( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e1_a = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_e1_a() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->e1_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_e2_a( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e2_a = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_e2_a() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->e2_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_e3_a( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e3_a = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_e3_a() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->e2_a;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_e1_b( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e1_b = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_e1_b() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->e1_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_e2_b( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e2_b = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_e2_b() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->e2_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_e3_b( const Vector3 & e )
{
    if ( joint != nullptr )
        joint->e3_b = Vector3d( at.x, at.y, at.z );
}

Vector3 JointNode::get_e3_b() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;
    
    const Vector3d & a = joint->e3_b;
    const Vector3 at( a.x_, a.y_, a.z_ );
    return at;
}

void JointNode::set_motor( bool en )
{
    if ( joint != nullptr )
        joint->motor = en;
}

bool JointNode::get_motor() const
{
    if ( jooint == nullptr )
        return false;
    return joint->motor;
}

void JointNode::set_compliance_joint( real_t k )
{
    if ( joint != nullptr )
        joint->compliance_joint = k;
}

real_t JointNode::get_compliance_joint() const
{
    if ( joint == nullptr )
        return 0.0;
    return joint->compliance_joint;
}

void JointNode::set_compliance_motor( real_t k )
{
    if ( joint != nullptr )
        joint->compliance_motor = k;
}

real_t JointNode::get_compliance_motor() const
{
    if ( joint == nullptr )
        return 0.0;
    return joint->compliance_motor;
}

Vector3 JointNode::get_force() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;

    const Vector3d & F = joint->force;
    const Vector3 f( F.x_, F.y_, F.z_ );
    return f;
}

Vector3 JointNode::get_torque() const
{
    if ( joint == nullptr )
        return Vector3::ZERO;

    const Vector3d & P = joint->torque;
    const Vector3 p( P.x_, P.y_, P.z_ );
    return p;
}



void JointNode::_notifications( int p_what )
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

void JointNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_body_path_a", "path" ), &PbdRigidBodyNode::set_body_path_a );
    ClassDB::bind_method( D_METHOD( "get_body_path_a" ),         &PbdRigidBodyNode::get_body_path_a, Variant::NODE_PATH );

    ClassDB::bind_method( D_METHOD( "set_body_path_b", "path" ), &PbdRigidBodyNode::set_body_path_b );
    ClassDB::bind_method( D_METHOD( "get_body_path_b" ),         &PbdRigidBodyNode::get_body_path_b, Variant::NODE_PATH );

    ClassDB::bind_method( D_METHOD( "set_at_a", "at" ), &PbdRigidBodyNode::set_at_a );
    ClassDB::bind_method( D_METHOD( "get_at_a" ),       &PbdRigidBodyNode::get_at_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_at_b", "at" ), &PbdRigidBodyNode::set_at_b );
    ClassDB::bind_method( D_METHOD( "get_at_b" ),       &PbdRigidBodyNode::get_at_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e1_a", "at" ), &PbdRigidBodyNode::set_e1_a );
    ClassDB::bind_method( D_METHOD( "get_e1_a" ),       &PbdRigidBodyNode::get_e1_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e2_a", "at" ), &PbdRigidBodyNode::set_e2_a );
    ClassDB::bind_method( D_METHOD( "get_e2_a" ),       &PbdRigidBodyNode::get_e2_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e3_a", "at" ), &PbdRigidBodyNode::set_e3_a );
    ClassDB::bind_method( D_METHOD( "get_e3_a" ),       &PbdRigidBodyNode::get_e3_a, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e1_b", "at" ), &PbdRigidBodyNode::set_e1_b );
    ClassDB::bind_method( D_METHOD( "get_e1_b" ),       &PbdRigidBodyNode::get_e1_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e2_b", "at" ), &PbdRigidBodyNode::set_e2_b );
    ClassDB::bind_method( D_METHOD( "get_e2_b" ),       &PbdRigidBodyNode::get_e2_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_e3_b", "at" ), &PbdRigidBodyNode::set_e3_b );
    ClassDB::bind_method( D_METHOD( "get_e3_b" ),       &PbdRigidBodyNode::get_e3_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_motor", "en" ), &PbdRigidBodyNode::set_motor );
    ClassDB::bind_method( D_METHOD( "get_motor" ),       &PbdRigidBodyNode::get_motor, Variant::BOOL );

    ClassDB::bind_method( D_METHOD( "set_compliance_joint", "k" ), &PbdRigidBodyNode::set_compliance_joint );
    ClassDB::bind_method( D_METHOD( "get_compliance_joint" ),      &PbdRigidBodyNode::get_compliance_joint, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_compliance_motor", "k" ), &PbdRigidBodyNode::set_compliance_motor );
    ClassDB::bind_method( D_METHOD( "get_compliance_motor" ),      &PbdRigidBodyNode::get_compliance_motor, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "get_force" ),  &PbdRigidBodyNode::get_force, Variant::VECTOR3 );
    ClassDB::bind_method( D_METHOD( "get_torque" ), &PbdRigidBodyNode::get_torque, Variant::VECTOR3 );




    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "body_path_a" ),    &PbdRigidBodyNode::set_body_path_a, &PbdRigidBodyNode::get_body_path_a );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "body_path_b" ),    &PbdRigidBodyNode::set_body_path_b, &PbdRigidBodyNode::get_body_path_b );

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "at_a" ),    &PbdRigidBodyNode::set_at_a, &PbdRigidBodyNode::get_at_a );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "at_b" ),    &PbdRigidBodyNode::set_at_b, &PbdRigidBodyNode::get_at_b );

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e1_a" ),    &PbdRigidBodyNode::set_e1_a, &PbdRigidBodyNode::get_e1_a );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e2_a" ),    &PbdRigidBodyNode::set_e2_a, &PbdRigidBodyNode::get_e2_a );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e3_a" ),    &PbdRigidBodyNode::set_e3_a, &PbdRigidBodyNode::get_e3_a );

    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e1_b" ),    &PbdRigidBodyNode::set_e1_b, &PbdRigidBodyNode::get_e1_b );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e2_b" ),    &PbdRigidBodyNode::set_e2_b, &PbdRigidBodyNode::get_e2_b );
    ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "e3_b" ),    &PbdRigidBodyNode::set_e3_b, &PbdRigidBodyNode::get_e3_b );


}

RigidBody * JointNode::body_from_path( const NodePath & path )
{
    Node * n = get_node( path );
    if ( n == nullptr )
        return nullptr;
    PbdRigidBodyNode * rbn = Node::cast_to<PbdRigidBodyNode>( n );
    if ( rbn == nullptr )
        return nullptr;

    return &(rbn->body);
}
 


}

