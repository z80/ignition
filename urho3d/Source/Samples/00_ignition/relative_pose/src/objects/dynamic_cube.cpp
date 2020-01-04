
#include "dynamic_cube.h"
#include "Global3dparty.h"

namespace Ign
{

void DynamicCube::RegisterComponent( Context * context )
{
    context->RegisterFactory<DynamicCube>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );

    URHO3D_ATTRIBUTE( "ThrustEnabled", bool, thrustEnabled_, false, AM_DEFAULT );
}

DynamicCube::DynamicCube( Context * context )
    : PhysicsItem( context )
{
    thrustEnabled_ = false;
}

DynamicCube::~DynamicCube()
{

}

void DynamicCube::ApplyControls( const Controls & ctrl )
{
    RigidBody2 * rb = rigidBody();
    if ( !rb )
        return;

    const float TORQUE = 0.1;
    const float THRUST = 1.2 * 10.0;
    const unsigned acts = ctrl.buttons_;
    if ( acts & CTRL_FORWARD )
        rb->ApplyTorque( Vector3::LEFT * TORQUE );
    else if ( acts & CTRL_BACK )
        rb->ApplyTorque( -Vector3::LEFT * TORQUE );
    else if ( acts & CTRL_LEFT )
        rb->ApplyTorque( Vector3::FORWARD * TORQUE );
    else if ( acts & CTRL_RIGHT )
        rb->ApplyTorque( -Vector3::FORWARD * TORQUE );
    if ( thrustEnabled_ )
    {
        rb->ApplyForce( Vector3::UP * THRUST );
    }
}

bool DynamicCube::IsSelectable() const
{
    return true;
}

void DynamicCube::Trigger( const VariantMap & data )
{
    (void)data;
    thrustEnabled_ = !thrustEnabled_;
}

void DynamicCube::createVisualContent( Node * n )
{
    if ( !n )
        return;

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    StaticModel * model = n->CreateComponent<StaticModel>( LOCAL );
    model->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Ign/Materials/TestCubeM.xml") );
}

void DynamicCube::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 1.0 );
    cs->SetBox( Vector3( 1.0, 1.0, 1.0 ) );
}


}


