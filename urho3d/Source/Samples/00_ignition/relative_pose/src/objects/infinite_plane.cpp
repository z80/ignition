
#include "infinite_plane.h"

namespace Ign
{

const int   InfinitePlane::QTY  = 20;
const Float InfinitePlane::STEP = 3.0;

void InfinitePlane::RegisterComponent( Context * context )
{
    context->RegisterFactory<InfinitePlane>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );
}

InfinitePlane::InfinitePlane( Context * context )
    : PhysicsItem( context )
{

}

InfinitePlane::~InfinitePlane()
{

}

Float InfinitePlane::distance( RefFrame * refFrame ) const
{
    if ( !refFrame )
        return -1.0;

    Vector3d rel_r;
    Quaterniond rel_q;
    this->relativePose( refFrame, rel_r, rel_q );
    const Vector3d a = rel_q * Vector3d( 0.0, 1.0, 0.0 );

    const Float d = rel_r.DotProduct( a );
    return d;
}

Float InfinitePlane::distance( const Vector3d & r ) const
{
    const Float d = r.y_;
    return d;
}

void InfinitePlane::refStateChanged()
{
    if ( !node_ )
        return;
    const Vector3d   at = refR();
    const Quaterniond q = refQ();

    visual_node_->SetPosition( Vector3( at.x_, at.y_, at.z_ ) );
    visual_node_->SetRotation( Quaternion( q.w_, q.x_, q.y_, q.z_ ) );
}

void InfinitePlane::createVisualContent( Node * node )
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();

    const Float r = STEP * static_cast<Float>(QTY-1)/2.0;
    for ( unsigned i=0; i<QTY; i++ )
    {
        const Float z = static_cast<Float>(i) * STEP - r;
        for ( unsigned j=0; j<QTY; j++ )
        {
            const Float x = static_cast<Float>(j) * STEP - r;
            const Vector3 at( x, -1.0, z );
            const String stri = String( "Sub element #" ) + String(i) + String(",") + String(j);
            Node * n = node->CreateChild( stri );
            n->SetPosition( at );
            StaticModel * m = n->CreateComponent<StaticModel>( LOCAL );

            m->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
            m->SetMaterial( cache->GetResource<Material>("Ign/Materials/TestCubeM.xml") );

            assignRefFrame( n );

            nodes_.Push( SharedPtr<Node>(n) );
        }
    }
}

void InfinitePlane::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 0.0 );
    cs->SetStaticPlane();
}

}

