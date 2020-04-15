
#include "physics_item.h"
#include "physics_frame.h"

// For debugging only. Drawing directions to all the planets.
#include "sphere_dynamic.h"

namespace Ign
{

void PhysicsItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<PhysicsItem>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

PhysicsItem::PhysicsItem( Context * context )
    : RefFrame( context )
{
}

PhysicsItem::~PhysicsItem()
{
}

void PhysicsItem::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    RefFrame::DrawDebugGeometry( debug, depthTest );
    if ( rigid_body_ )
        rigid_body_->DrawDebugGeometry( debug, depthTest );
    if ( collision_shape_ )
        collision_shape_->DrawDebugGeometry( debug, depthTest );
    if ( visual_node_ )
        air_mesh_.drawDebugGeometry( visual_node_, debug );

    // Draw directions to all the planets.
    {
        if ( parent_id_ == 0 )
            return;

        Scene * s = GetScene();
        const Vector<SharedPtr<Component> > & comps = s->GetComponents();
        const unsigned qty = comps.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            Component * c = comps[i];
            SphereDynamic * sd = c->Cast<SphereDynamic>();
            if ( !sd )
                continue;
            State st;
            sd->relativeState( parent_id_, st );
            st.r.Normalize();
            const Vector3d origin = relR();
            Vector3 from( origin.x_, origin.y_, origin.z_ );
            const Float SZ = 10.0;
            Vector3 to( st.r.x_*SZ, st.r.y_*SZ, st.r.z_*SZ );
            to += from;
            debug->AddLine( from, to, Color::RED, depthTest );
        }
    }
}

void PhysicsItem::setR( const Vector3d & r )
{
    RefFrame::setR( r );
    if ( rigid_body_ )
        rigid_body_->SetPositiond( r );
}

void PhysicsItem::setQ( const Quaterniond & q )
{
    RefFrame::setQ( q );
    if ( rigid_body_ )
        rigid_body_->SetRotationd( q );
}

void PhysicsItem::setV( const Vector3d & v )
{
    RefFrame::setV( v );
    if ( rigid_body_ )
        rigid_body_->SetLinearVelocityd( v );
}

void PhysicsItem::setW( const Vector3d & w )
{
    RefFrame::setW( w );
    if ( rigid_body_ )
        rigid_body_->SetAngularVelocityd( w );
}

void PhysicsItem::refStateChanged()
{
    const Vector3    r = refR().vector3();
    const Quaternion q = refQ().quaternion();
    visual_node_->SetTransform( r, q );
}

void PhysicsItem::updateStateFromRigidBody()
{
    if ( !rigid_body_ )
        return;
    physicsUpdate( rigid_body_ );
    // Assign state variables from rigid body state.
    st_.r = rigid_body_->GetPositiond();
    st_.q = rigid_body_->GetRotationd();
    st_.v = rigid_body_->GetLinearVelocityd();
    st_.w = rigid_body_->GetAngularVelocityd();

    MarkNetworkUpdate();
}

RigidBody2 * PhysicsItem::rigidBody() const
{
    return rigid_body_;
}

AirMesh & PhysicsItem::airMesh()
{
    return air_mesh_;
}

void PhysicsItem::enteredRefFrame( unsigned refFrameId )
{
    Environment * env_ = this->env();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;


    // Check if it is a physics ref frame.
    // And if yes create physics content.
    if ( refFrameId == 0 )
        return;
    RefFrame * rf = refFrame( refFrameId );
    if ( !rf )
        return;
    PhysicsFrame * pf = rf->Cast<PhysicsFrame>();
    if ( !pf )
        return;
    Node * node = pf->physicsNode();
    if ( !node )
        return;
    const String n = String( this->name() ) + String( " physics node" );
    physics_node_    = node->CreateChild( n, LOCAL );
    rigid_body_      = physics_node_->CreateComponent<RigidBody2>( LOCAL );
    collision_shape_ = physics_node_->CreateComponent<CollisionShape2>( LOCAL );
    
    setupPhysicsContent( rigid_body_, collision_shape_ );

    // Assign state to rigid body.
    rigid_body_->SetPositiond( Vector3( st_.r.x_, st_.r.y_, st_.r.z_ ) );
    rigid_body_->SetRotationd( Quaternion( st_.q.w_, st_.q.x_, st_.q.y_, st_.q.z_ ) );
    rigid_body_->SetLinearVelocityd( Vector3( st_.v.x_, st_.v.y_, st_.v.z_ ) );
    rigid_body_->SetAngularVelocityd( Vector3( st_.w.x_, st_.w.y_, st_.v.z_ ) );
}

void PhysicsItem::leftRefFrame( unsigned refFrameId )
{
    // Need to destroy physics content.
    if ( physics_node_ )
        physics_node_->Remove();
}

void PhysicsItem::childEntered( unsigned refFrameId )
{
    // I commented these code out as I don't remember what logic
    // I meant when put it here. But from the code I don't quite understand
    // what's going on.

    // Actually, this code created physics environment when
    // camera is attached to this physics item.


}

void PhysicsItem::childLeft( unsigned refFrameId )
{
}

void PhysicsItem::focusedByCamera( unsigned cameraFrameId )
{
    RefFrame::focusedByCamera( cameraFrameId );
    // Actually, this code created physics environment when
    // camera is attached to this physics item.

    Scene * scene = GetScene();
    if ( !scene )
        return;

    Environment * env_ = this->env();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;

    // Check if parent is physics frame.
    RefFrame * prt = parent();
    if ( prt )
    {
        PhysicsFrame * pf = prt->Cast<PhysicsFrame>();
        if ( pf )
            return;
    }

    // Prior to creating physical environment check its parent.
    // If parent enforces all children to be kinematic, 
    // do not create physics environment.
    const bool kinematic = shouldBeKinematic();
    if ( kinematic )
        return;

    // For simplicity just create new physics frame and place this object into it.
    // Physics frame will take all the logic load concerning if it needs to merge
    // with anything, etc.
    PhysicsFrame * pf = scene->CreateComponent<PhysicsFrame>();
    pf->setParent( parent_id_ );
    pf->setState( this->state() );
    this->setParent( pf );
}

void PhysicsItem::OnSceneSet( Scene * scene )
{
    if ( !scene )
    {
        if ( visual_node_ )
            visual_node_->Remove();
        return;
    }
    
    // Create visuals to the "visual_node_"
    const String n = this->name() + " visual node";
    visual_node_ = scene->CreateChild( n );
    assignRefFrame( visual_node_ );
    createVisualContent( visual_node_ );
}


void PhysicsItem::createVisualContent( Node * n )
{
}

void PhysicsItem::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
}

void PhysicsItem::physicsUpdate( RigidBody2 * rb )
{
}


}





