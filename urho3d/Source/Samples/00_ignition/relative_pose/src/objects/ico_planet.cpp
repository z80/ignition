
#include "ico_planet.h"
#include "settings.h"

namespace Ign
{

const Float IcoPlanet::R_ = 10.0;

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

Float IcoPlanet::distance( RefFrame * refFrame ) const
{
    Vector3d rel_r;
    Quaterniond rel_q;
    relativePose( refFrame, rel_r, rel_q );
    const Float d = rel_r.Length();
    const Float dist = ( d > R_ ) ? (d - R_) : 0.0;
    return dist;
}

Float IcoPlanet::distance( const Vector3d & r ) const
{
    const Float d = r.Length();
    const Float dist = ( d > R_ ) ? (d - R_) : 0.0;
    return dist;
}

void IcoPlanet::refStateChanged()
{
    if ( !node_ )
        return;

    const Vector3d    r0 = refR();
    const Quaterniond q = refQ();

    Float scale  = R_;
    Float dScale = 0.7;
    const Float maxDist = Settings::staticObjDistanceHorizontShow();
    Vector3d r = r0;
    while ( r.Length() > maxDist )
    {
        scale *= dScale;
        r = r0 * scale;
    }

    visual_node_->SetPosition( Vector3( r.x_, r.y_, r.z_ ) );
    visual_node_->SetRotation( Quaternion( q.w_, q.x_, q.y_, q.z_ ) );
    visual_node_->SetScale( scale );
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

    cs->SetGImpactMesh( m, R_ );
}



}


