
#include "ico_planet.h"
#include "settings.h"

void IcoPlanet::RegisterComponent( Context * context )
{
    context->RegisterFactory<IcoPlanet>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );

}

IcoPlanet::IcoPlanet( Context * context )
    : PhysicsItem( context )
{

}

IcoPlanet::~IcoPlanet()
{

}

void IcoPlanet::refStateChanged()
{
    if ( !node_ )
        return;

    const Vector3d    r0 = refR();
    const Quaterniond q = refQ();

    Float scale  = 1.0;
    Float dScale = 0.7;
    const Float maxDist = Settings::staticObjDistanceHorizontShow();
    Vector3d r = r0;
    while ( r.Length() > maxDist )
    {
        scale *= dScale;
        r = r0 * scale;
    }

    node_->SetPosition( Vector3( r.x_, r.y_, r.z_ ) );
    node_->SetRotation( Quaternion( q.w_, q.x_, q.y_, q.z_ ) );
    node_->SetScale( scale );
}

void IcoPlanet::createVisualContent( Node * n )
{
    URHO3D_LOGINFOF( "IcoPlanet::createVisualContent()" );

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    const String name = name_ + "_node";
    n->SetName( name );

    StaticModel * model = n->CreateComponent<StaticModel>( LOCAL );
    model->SetModel( cache->GetResource<Model>("Ign/Models/IcoPlanet.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Ign/Materials/IcoPlanetM.xml") );

}

void IcoPlanet::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 0.0 );

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    Model * m = cache->GetResource<Model>("Ign/Models/IcoPlanet.mdl");

    cs->SetGImpactMesh( m );
}




