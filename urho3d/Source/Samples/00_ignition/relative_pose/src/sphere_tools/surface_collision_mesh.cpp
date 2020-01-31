

#include "surface_collision_mesh.h"

void SurfaceCollisionMesh::RegisterComponent( Context * context )
{
    context->RegisterFactory<SurfaceCollisionMesh>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );
}


SurfaceCollisionMesh::SurfaceCollisionMesh( Context * context )
    : PhysicsItem( context )
{
}

SurfaceCollisionMesh::~SurfaceCollisionMesh()
{
}

void SurfaceCollisionMesh::parentTeleported()
{
    // Recompute dynamic geometry.
}

bool SurfaceCollisionMesh::IsSelectable() const
{
    return false;
}

void SurfaceCollisionMesh::createVisualContent( Node * n )
{
    if ( !n )
        return;

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    StaticModel * model = n->CreateComponent<StaticModel>( LOCAL );
    model->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Ign/Materials/TestCubeM.xml") );
}

void SurfaceCollisionMesh::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 0.0 );
    // Here need to specify custom geometry based one later.
    //cs->SetBox( Vector3( 1.0, 1.0, 1.0 ) );
    //cs->SetCustomGImpactMesh(  );
}

SphereItem * SurfaceCollisionMesh::pickSphere()
{

    return nullptr;
}


















