
#include "infinite_plane.h"

namespace Ign
{

const int   InfinitePlane::QTY  = 20;
const Float InfinitePlane::STEP = 3.0;

void InfinitePlane::RegisterComponent( Context * context )
{
    context->RegisterFactory<InfinitePlane>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

InfinitePlane::InfinitePlane( Context * context )
    : RefFrame( context )
{

}

InfinitePlane::~InfinitePlane()
{

}

void InfinitePlane::refStateChanged()
{
    if ( !node_ )
        return;
    const Vector3d   at = refR();
    const Quaterniond q = refQ();

    node_->SetPosition( Vector3( at.x_, at.y_, at.z_ ) );
    node_->SetRotation( Quaternion( q.w_, q.x_, q.y_, q.z_ ) );
}

void InfinitePlane::poseChanged()
{

}

void InfinitePlane::OnSceneSet( Scene * scene )
{
    Scene * s = GetScene();
    if ( !s )
    {
        node_.Reset();
        return;
    }

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    node_ = s->CreateChild( String( "Infinite Plane" ), LOCAL );
    const Float r = STEP * static_cast<Float>(QTY-1)/2.0;
    for ( unsigned i=0; i<QTY; i++ )
    {
        const Float z = static_cast<Float>(i) * STEP - r;
        for ( unsigned j=0; j<QTY; j++ )
        {
            const Float x = static_cast<Float>(j) * STEP - r;
            const Vector3 at( x, -1.0, z );
            const String stri = String( "Sub element #" ) + String(i) + String(",") + String(j);
            Node * n = node_->CreateChild( stri );
            n->SetPosition( at );
            StaticModel * m = n->CreateComponent<StaticModel>( LOCAL );

            m->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
            m->SetMaterial( cache->GetResource<Material>("Ign/Materials/TestCubeM.xml") );

            assignRefFrame( n );
        }
    }

    // To be able to retrieve RefFrame.
    assignRefFrame( node_ );

    computeRefState( nullptr, 0 );
}

}

