

#include "surface_collision_mesh.h"

void SurfaceCollsionMesh::RegisterComponent( Context * context )
{
    context->RegisterFactory<DynamicCube>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );
}


SurfaceCollsionMesh::SurfaceCollsionMesh( Context * context )
    : PhysicsItem( context )
{
}

SurfaceCollsionMesh::~SurfaceCollsionMesh()
{
}

bool SurfaceCollsionMesh::IsSelectable() const
{
    return false;
}

void SurfaceCollsionMesh::createVisualContent( Node * n )
{
    if ( !n )
        return;

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    StaticModel * model = n->CreateComponent<StaticModel>( LOCAL );
    model->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Ign/Materials/TestCubeM.xml") );
}
}

void SurfaceCollsionMesh::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 0.0 );
    // Here need to specify custom geometry based one later.
    cs->SetBox( Vector3( 1.0, 1.0, 1.0 ) );
}


















