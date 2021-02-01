
#include "pbd2_rigid_body_node.h"
#include "pbd2_simulation_node.h"
#include "pbd2_contact_point_node.h"

#include "core/engine.h"

namespace Pbd
{

static void apply_rigid_body_pose( PbdRigidBodyNode * rbn )
{
    if ( rbn == nullptr )
            return;
    const RigidBody & rb = rbn->rigid_body;
    const Vector3d & r_ = rb.pose.r;
    const Quaterniond & q_ = rb.pose.q;
    const Vector3 r( r_.x_, r_.y_, r_.z_ );
    const Quat q( q_.x_, q_.y_, q_.z_, q_.w_ );

    Transform t = rbn->get_transform();
    t.set_origin( r );
    t.set_basis( q );
    rbn->set_transform( t );
}

static void enter_tree( PbdRigidBodyNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulationNode>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.bodies.push_back( &(rbn->rigid_body) );
}

static void exit_tree( PbdRigidBodyNode * rbn )
{
    Node * n = rbn->get_parent();
    PbdSimulationNode * sn = Node::cast_to<PbdSimulationNode>( n );
    if ( sn == nullptr )
        return;
    Simulation & s = sn->simulation;
    s.bodies.erase( &(rbn->rigid_body) );
}


PbdRigidBodyNode::PbdRigidBodyNode()
    : Spatial()
{
    set_process( true );
    set_notify_local_transform( true );
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
    i.m00_ = I.elements[0].x;
    i.m10_ = I.elements[0].y;
    i.m20_ = I.elements[0].z;
    i.m01_ = I.elements[1].x;
    i.m11_ = I.elements[1].y;
    i.m21_ = I.elements[1].z;
    i.m02_ = I.elements[2].x;
    i.m12_ = I.elements[2].y;
    i.m22_ = I.elements[2].z;
    // It also computes inverted inertia.
    rigid_body.set_inertia( i );
}

Basis PbdRigidBodyNode::get_inertia() const
{
    const Matrix3d & I = rigid_body.inertia;
    Basis i;
    i.elements[0].x = I.m00_;
    i.elements[0].y = I.m10_;
    i.elements[0].z = I.m20_;
    i.elements[1].x = I.m01_;
    i.elements[1].y = I.m11_;
    i.elements[1].z = I.m21_;
    i.elements[2].x = I.m02_;
    i.elements[2].y = I.m12_;
    i.elements[2].z = I.m22_;
    return i;
}

void PbdRigidBodyNode::set_transform_rb( const Transform & t )
{
    const Vector3 & o = t.origin;
    const Quat q = t.basis.get_quat();
    rigid_body.pose.r = Vector3d( o.x, o.y, o.z );
    rigid_body.pose.q = Quaterniond( q.w, q.x, q.y, q.z );
}

//Transform PbdRigidBodyNode::get_transform_rb() const
//{
//      const Vector3d & o = rigid_body.pose.r;
//      const Quaterniond & q = rigid_body.pose.q;
//      Transform t;
//      t.origin = Vector3( o.x_, o.y_, o.z_ );
//      t.basis = Quat( q.x_, q.y_, q.z_, q.w_ );
//    return t;
//}

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

void PbdRigidBodyNode::set_damping_linear( real_t k )
{
        rigid_body.damping_linear = k;
}

real_t PbdRigidBodyNode::get_damping_linear() const
{
        return rigid_body.damping_linear;
}

void PbdRigidBodyNode::set_damping_angular( real_t k )
{
        rigid_body.damping_angular = k;
}

real_t PbdRigidBodyNode::get_damping_angular() const
{
        return rigid_body.damping_angular;
}

void PbdRigidBodyNode::set_compliance_normal( real_t k )
{
	rigid_body.compliance_normal = k;
}

real_t PbdRigidBodyNode::get_compliance_normal() const
{
	return rigid_body.compliance_normal;
}

void PbdRigidBodyNode::set_compliance_tangential( real_t k )
{
	rigid_body.compliance_tangential = k;
}

real_t PbdRigidBodyNode::get_compliance_tangential() const
{
	return rigid_body.compliance_tangential;
}



void PbdRigidBodyNode::set_force( const Vector3 & f )
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



void PbdRigidBodyNode::_notification( int p_what )
{
    switch ( p_what )
    {
        case NOTIFICATION_PROCESS:
            if ( !Engine::get_singleton()->is_editor_hint() )
                apply_rigid_body_pose( this );        
            break;

        case NOTIFICATION_ENTER_TREE:
            enter_tree( this );
            break;

        case NOTIFICATION_EXIT_TREE:
            exit_tree( this );

        case NOTIFICATION_LOCAL_TRANSFORM_CHANGED:
            set_transform_rb( get_transform() );
            break;

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

    //ClassDB::bind_method( D_METHOD( "set_transform_rb", "t" ), &PbdRigidBodyNode::set_transform_rb );
    //ClassDB::bind_method( D_METHOD( "get_transform_rb" ),      &PbdRigidBodyNode::get_transform_rb, Variant::TRANSFORM );

    ClassDB::bind_method( D_METHOD( "set_linear_velocity", "v" ), &PbdRigidBodyNode::set_linear_velocity );
    ClassDB::bind_method( D_METHOD( "get_linear_velocity" ),      &PbdRigidBodyNode::get_linear_velocity, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_angular_velocity", "w" ), &PbdRigidBodyNode::set_angular_velocity );
    ClassDB::bind_method( D_METHOD( "get_angular_velocity" ),      &PbdRigidBodyNode::get_angular_velocity, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_friction", "k" ), &PbdRigidBodyNode::set_friction );
    ClassDB::bind_method( D_METHOD( "get_friction" ),      &PbdRigidBodyNode::get_friction, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_restitution", "k" ), &PbdRigidBodyNode::set_restitution );
    ClassDB::bind_method( D_METHOD( "get_restitution" ),      &PbdRigidBodyNode::get_restitution, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_damping_linear", "k" ), &PbdRigidBodyNode::set_damping_linear );
    ClassDB::bind_method( D_METHOD( "get_damping_linear" ),      &PbdRigidBodyNode::get_damping_linear, Variant::REAL );

    ClassDB::bind_method( D_METHOD( "set_damping_angular", "k" ), &PbdRigidBodyNode::set_damping_angular );
    ClassDB::bind_method( D_METHOD( "get_damping_angular" ),      &PbdRigidBodyNode::get_damping_angular, Variant::REAL );

	ClassDB::bind_method( D_METHOD( "set_compliance_normal", "k" ), &PbdRigidBodyNode::set_compliance_normal );
	ClassDB::bind_method( D_METHOD( "get_compliance_normal" ),      &PbdRigidBodyNode::get_compliance_normal, Variant::REAL );

	ClassDB::bind_method( D_METHOD( "set_compliance_tangential", "k" ), &PbdRigidBodyNode::set_compliance_tangential );
	ClassDB::bind_method( D_METHOD( "get_compliance_tangential" ),      &PbdRigidBodyNode::get_compliance_tangential, Variant::REAL );


    ClassDB::bind_method( D_METHOD( "set_force", "f" ), &PbdRigidBodyNode::set_force );
    ClassDB::bind_method( D_METHOD( "get_force" ),      &PbdRigidBodyNode::get_force, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_torque", "p" ), &PbdRigidBodyNode::set_torque );
    ClassDB::bind_method( D_METHOD( "get_torque" ),      &PbdRigidBodyNode::get_torque, Variant::VECTOR3 );


    ADD_PROPERTY( PropertyInfo( Variant::REAL, "mass" ),    "set_mass", "get_mass" );
    ADD_PROPERTY( PropertyInfo( Variant::BASIS, "inertia" ), "set_inertia", "get_inertia" );
    //ADD_PROPERTY( PropertyInfo( Variant::TRANSFORM, "transform_rb" ), "set_transform_rb", "get_transform_rb" );
    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "linear_velocity" ), "set_linear_velocity", "get_linear_velocity" );
    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "angular_velocity" ), "set_angular_velocity", "get_angular_velocity" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "friction" ), "set_friction", "get_friction" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "restitution" ), "set_restitution", "get_restitution" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "damping_linear" ), "set_damping_linear", "get_damping_linear" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "damping_angular" ), "set_damping_angular", "get_damping_angular" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "compliance_normal" ), "set_compliance_normal", "get_compliance_normal" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "compliance_tangential" ), "set_compliance_tangential", "get_compliance_tangential" );
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "force" ), "set_force", "get_force" );
    ADD_PROPERTY( PropertyInfo( Variant::VECTOR3, "torque" ), "set_torque", "get_torque" );
}




}










