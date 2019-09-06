
#include "thruster.h"
#include "tech_tree.h"
//#include "Urho3D/Physics/RigidBody.h"
//#include "Urho3D/Physics/CollisionShape.h"
#include "rigid_body_2.h"
#include "collision_shape_2.h"

namespace Osp
{

Thruster::Thruster( Context * c )
    : Block( c, "Thruster" )
{
}

Thruster::~Thruster()
{

}

void Thruster::createContent( Node * node )
{
    Block::createContent( node );

    //n->CreateChild();
    StaticModel * obj = node->CreateComponent<StaticModel>();

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    obj->SetModel( cache->GetResource<Model>( "Models/Thruster.mdl" ) );
    obj->SetMaterial( cache->GetResource<Material>( "Materials/Thruster_M_2.xml" ) );
    obj->SetCastShadows( true );

    airMesh.init( obj );
}

void Thruster::toWorld()
{
    Block::toWorld();

    Node * node = GetNode();

    RigidBody2 * rb = node->CreateComponent<RigidBody2>();
    rb->SetMass( 1.0 );

    CollisionShape2 * cs = node->CreateComponent<CollisionShape2>();
    cs->SetCapsule( 1.0, 2.5 );

    /*{
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        Model * m = cache->GetResource<Model>( "Models/Thruster.mdl" );
        cs->SetConvexHull( m, 1 );
    }*/
}

void Thruster::fromWorld()
{
    Block::fromWorld();
}



}


