
#include "character_cube.h"
#include "Global3dparty.h"

namespace Ign
{

void CharacterCube::RegisterComponent( Context * context )
{
    context->RegisterFactory<CharacterCube>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsCharacterItem );
}

CharacterCube::CharacterCube( Context * context )
    : PhysicsCharacterItem( context )
{
    setName( "CharacterCube" );
}

CharacterCube::~CharacterCube()
{

}

void CharacterCube::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    PhysicsCharacterItem::DrawDebugGeometry( debug, depthTest );
}

void CharacterCube::ApplyControls( const Controls & ctrl )
{
    const Float MOVE_FORCE = 100.0;

    if ( !rigid_body_ )
        return;

    Quaternion q = rigid_body_->GetRotation();

    // Movement torque is applied before each simulation step, which happen at 60 FPS. This makes the simulation
    // independent from rendering framerate. We could also apply forces (which would enable in-air control),
    // but want to emphasize that it's a ball which should only control its motion by rolling along the ground
    if ( ctrl.buttons_ & CTRL_FORWARD )
        rigid_body_->ApplyForce( q * Vector3::FORWARD * MOVE_FORCE );
    else if ( ctrl.buttons_ & CTRL_BACK )
        rigid_body_->ApplyForce( q * Vector3::BACK * MOVE_FORCE );
    else if ( ctrl.buttons_ & CTRL_LEFT )
        rigid_body_->ApplyForce( q * Vector3::LEFT * MOVE_FORCE );
    else if ( ctrl.buttons_ & CTRL_RIGHT )
        rigid_body_->ApplyForce( q * Vector3::RIGHT * MOVE_FORCE );
}

bool CharacterCube::AcceptsControls( int userId ) const
{
    return true;
}

bool CharacterCube::IsSelectable() const
{
    return true;
}

void CharacterCube::Trigger( const VariantMap & data )
{

}

void CharacterCube::createVisualContent( Node * n )
{
    if ( !n )
        return;

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    StaticModel * model = n->CreateComponent<StaticModel>( LOCAL );
    model->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Ign/Materials/TestCubeM.xml") );
}

void CharacterCube::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    PhysicsCharacterItem::setupPhysicsContent( rb, cs );
    rb->SetMass( 1.0f );
    rb->SetFriction( 1.0f );
    // In addition to friction, use motion damping so that the ball can not accelerate limitlessly
    rb->SetLinearDamping( 0.5f );

    cs->SetBox( Vector3( 1.0, 1.0, 1.0 ) );

    Scene * s = GetScene();
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    StaticModel * m = s->CreateComponent<StaticModel>( LOCAL );
    m->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
    air_mesh_ = m;
    m->Remove();
}

}



