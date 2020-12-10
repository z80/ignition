
#include "pbd2_rigid_body_node.h"
#include "pbd2_simulation_node.h"

namespace Pbd
{

static void apply_rigid_body_pose( PbdRigidBodyNode * rbn )
{
    if ( rbd == nullptr )
            return;
    const RigidBody & rb = rbn->rigid_body;
    const Vector3d & r_ = rb.pose.r
    const Quaterniond & q_ = rb.pose.q
    const Vector3 r( r_.x_, r_.y_, r_.z_ );
    const Quat q( q_.x_, q_.y_, q_.z_, q_.w_ );

    Transform t = get_transform();
    t.set_origin( r );
    t.set_basis( q );
    set_transform( t );
}

static void enter_tree( PbdRigidBodyNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulation>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.bodies.push_back( &(rbn->rigid_body) );
}

static void exit_tree( PbdRigidBodyNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulation>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.bodies.erase( &(rbn->rigid_body) );
}


PbdRigidBodyNode::PbdRigidBodyNode()
    : Spatial()
{
    set_process( true );
}

PbdRigidBodyNode::~PbdRigidBodyNode()
{
}

void PbdRigidBodyNode::set_mass( real_t m )
{
    rigid_body.mass = m;
}

real_t PbdRigidBodyNode::get_mass() const
{
    return rigid_body.mass;
}

void PbdRigidBodyNode::set_inertia( const Basis & I )
{
    Matrix3d i;
    rigid_body.inertia = i;
}

Basis PbdRigidBodyNode::get_inertia() const
{
    Basis I = Basis::IDENTITY;
    rigid_body.inertia;
    return I;
}

void PbdRigidBodyNode::set_transform_rb( const Transform & t )
{
}

Transform PbdRigidBodyNode::get_transform_rb() const
{
    Transform t = Transform::IDENTITY;
    return Transform;
}

void PbdRigidBodyNode::set_linear_velocity( const Vector3 & v )
{
    const Vector3d V( v.x, v.y, v.z );
    rigid_body.vel = V;
}

Vector3 PbdRigidBodyNode::get_linear_velocity() const
{
    const Vector3d & V = rigid_body.vel;
    Vector3 v( V.x_, V.y_, V.z_ );
    return v;
}

void PbdRigidBodyNode::set_angular_velocity( const Vector3 & w )
{
    const Vector3d W( w.x, w.y, w.z );
    rigid_body.omega = W;
}

Vector3 PbdRigidBodyNode::get_angular_velocity() const
{
    const Vector3d & W = rigid_body.omega;
    const Vector3 w( W.x_, W.y_, W.z_ );
    return w;
}

void PbdRigidBodyNode::set_friction( real_t k )
{
    rigid_body.friction = k;
}

real_t PbdRigidBodyNode::get_friction() const
{
    return rigid_body.friction;
}

void PbdRigidBodyNode::set_restitution( real_t k )
{
    rigid_body.restitution = k;
}

real_t PbdRigidBodyNode::get_restitution() const
{
    return rigid_body.restitution;
}

void PbdRigidBodyNode::set_force( cosnt Vector3 & f )
{
    const Vector3d F( f.x, f.y, f.z );
    rigid_body.force = F;
}

Vector3 PbdRigidBodyNode::get_force() const
{
    const Vector3d & F = rigid_body.force;
    const Vector3 f( F.x_, F.y_, F.z_ );
    return f;
}

void PbdRigidBodyNode::set_torque( const Vector3 & p )
{
    const Vector3d P( p.x, p.y, p.z );
    rigid_body.torque = P;
}

Vector3 PbdRigidBodyNode::get_torque() const
{
    const Vector3d & P = rigid_body.torque;
    const Vector3 p( P.x_, P.y_, P.z_ );
    return p;
}



void PbdRigidBodyNode::rebuild_contacts()
{
    rigid_body.points.clear();
    const int qty = get_children_count();
    for ( int i=0; i<qty; i++ )
    {
        Node * n = get_child( i );
        PbdContactPointNode * cpn = Node::cast_to<PbdContactPointNode>( n );
        if ( cpn != nullptr )
        {
            Vector3 at = cpn->get_transform.origin();
            ContactPoint cp;
            cp.r = Vector3d( at.x, at.y, at.z );
            rigid_body.points.push_back( cp );
        }
    }
}

void PbdRigidBodyNode::_notifications( int p_what )
{
    Spatial::_notifications( p_what );

    switch ( p_what )
    {
        case NOTIFICATION_READY:
            rebuild_contacts();
            break;

        case NOTIFICATION_PROCESS:
            apply_rigid_body_pose( this );        
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

void PbdRigidBodyNode::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_mass", "m" ), &PbdRigidBodyNode::set_mass );
    ClassDB::bind_method( D_METHOD( "get_mass" ),      &PbdRigidBodyNode::get_mass, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_inertia", "I" ), &PbdRigidBodyNode::set_inertia );
    ClassDB::bind_method( D_METHOD( "get_inertia" ),      &PbdRigidBodyNode::get_inertia, Variant::BASIS );

    ClassDB::bind_method( D_METHOD( "set_transform_rb", "t" ), &PbdRigidBodyNode::set_transform_rb );
    ClassDB::bind_method( D_METHOD( "get_transform_rb" ),      &PbdRigidBodyNode::get_transform_rb, Variant::TRANSFORM );

    ClassDB::bind_method( D_METHOD( "set_linear_velocity", "v" ), &PbdRigidBodyNode::set_linear_velocity );
    ClassDB::bind_method( D_METHOD( "get_linear_velocity" ),      &PbdRigidBodyNode::get_linear_velocity, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_angular_velocity", "w" ), &PbdRigidBodyNode::set_angular_velocity );
    ClassDB::bind_method( D_METHOD( "get_angular_velocity" ),      &PbdRigidBodyNode::get_angular_velocity, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_friction", "k" ), &PbdRigidBodyNode::set_friction );
    ClassDB::bind_method( D_METHOD( "get_friction" ),      &PbdRigidBodyNode::get_friction, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_restitution", "k" ), &PbdRigidBodyNode::set_restitution );
    ClassDB::bind_method( D_METHOD( "get_restitution" ),      &PbdRigidBodyNode::get_restitution, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_force", "f" ), &PbdRigidBodyNode::set_force );
    ClassDB::bind_method( D_METHOD( "get_force" ),      &PbdRigidBodyNode::get_force, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_torque", "p" ), &PbdRigidBodyNode::set_torque );
    ClassDB::bind_method( D_METHOD( "get_torque" ),      &PbdRigidBodyNode::get_torque, Variant::VECTOR3 );


    ADD_PROPERTY( PropertyInfo( Variant::REAL, "mass" ),    &PbdRigidBodyNode::set_mass, &PbdRigidBodyNode::get_mass );
    ADD_PROPERTY( PropertyInfo( Variant::BASIS, "inertia" ), &PbdRigidBodyNode::set_inertia, &PbdRigidBodyNode::get_inertia );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "transform_rb" ), &PbdRigidBodyNode::set_transform_rb, &PbdRigidBodyNode::get_transform_rb );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "linear_velocity" ), &PbdRigidBodyNode::set_linear_velcoity, &PbdRigidBodyNode::get_linear_velocity );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "angular_velocity" ), &PbdRigidBodyNode::set_angular_velcoity, &PbdRigidBodyNode::get_angular_velocity );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "friction" ), &PbdRigidBodyNode::set_friction, &PbdRigidBodyNode::get_friction );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "restitution" ), &PbdRigidBodyNode::set_restitution, &PbdRigidBodyNode::get_restitution );
    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "force" ), &PbdRigidBodyNode::set_force, &PbdRigidBodyNode::get_force );
    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "torque" ), &PbdRigidBodyNode::set_torque, &PbdRigidBodyNode::get_torque );
}




}










