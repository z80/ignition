
#include "static_mesh.h"

namespace Ign
{


void StaticMesh::RegisterComponent( Context * context )
{
    context->RegisterFactory<StaticMesh>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

StaticMesh::StaticMesh( Context * ctx )
    : RefFrame( ctx )
{
    URHO3D_LOGINFOF( "StaticMesh::StaticMesh()" );
}

StaticMesh::~StaticMesh()
{
    URHO3D_LOGINFOF( "StaticMesh::~StaticMesh()" );
}

void StaticMesh::refStateChanged()
{
    const Vector3    r = refR().vector3();
    const Quaternion q = refQ().quaternion();
    node_->SetTransform( r, q );
}

void StaticMesh::poseChanged()
{
    computeRefState( nullptr );
    URHO3D_LOGINFOF( "StaticMesh::poseChanged()" );
}

void StaticMesh::OnNodeSet( Node * node )
{
    URHO3D_LOGINFOF( "StaticMesh::OnNodeSet()" );
}

void StaticMesh::OnSceneSet( Scene * scene )
{
    URHO3D_LOGINFOF( "StaticMesh::OnSceneSet()" );

    if ( !scene )
        return;

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    const String name = name_ + "_node";
    node_ = SharedPtr<Node>( scene->CreateChild( name, LOCAL ) );

    StaticModel * model = node_->CreateComponent<StaticModel>( LOCAL );
    model->SetModel( cache->GetResource<Model>("Models/Mushroom.mdl") );
    model->SetMaterial( cache->GetResource<Material>("Materials/Mushroom.xml") );

    computeRefState( nullptr, 0 );
}

void StaticMesh::OnMarkedDirty( Node * node )
{
    URHO3D_LOGINFOF( "StaticMesh::OnMarkedDirty()" );
}

void StaticMesh::OnNodeSetEnabled( Node * node )
{
    URHO3D_LOGINFOF( "StaticMesh::OnNodeSetEnabled()" );
}


}






