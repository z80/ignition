
#include "physics_item.h"
#include "physics_frame.h"



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

const AirMesh & PhysicsItem::airMesh() const
{
    return air_mesh_;
}

void PhysicsItem::enteredRefFrame( RefFrame * refFrame )
{
    Environment * env_ = this->env();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;


    // Check if it is a physics ref frame.
    // And if yes create physics content.
    if ( !refFrame )
        return;
    PhysicsFrame * pf = refFrame->Cast<PhysicsFrame>();
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

void PhysicsItem::leftRefFrame( RefFrame * refFrame )
{
    // Need to destroy physics content.
    if ( physics_node_ )
        physics_node_->Remove();
}

void PhysicsItem::childEntered( RefFrame * refFrame )
{
    // I commented these code out as I don't remember what logic
    // I meant when put it here. But from the code I don't quite understand
    // what's going on.

    // Actually, this code created physics environment when
    // camera is attached to this physics item.

    Scene * scene = GetScene();
    if ( !scene )
        return;

    const bool controlledByUser = getUserControlled();
    if ( !controlledByUser )
        return;

    Environment * env_ = this->env();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;

    // Check if parent is physics frame.
    if ( parent_ )
    {
        PhysicsFrame * pf = parent_->Cast<PhysicsFrame>();
        if ( pf )
            return;
    }

    // For simplicity just create new physics frame and place this object into it.
    // Physics frame will take all the logic load concerning if it needs to merge
    // with anything, etc.
    PhysicsFrame * pf = scene->CreateComponent<PhysicsFrame>();
    pf->setParent( this->parent_ );
    pf->setState( this->state() );
    this->setParent( pf );
}

void PhysicsItem::childLeft( RefFrame * refFrame )
{

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





