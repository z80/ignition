
#include "box.h"
#include "tech_tree.h"
//#include "Urho3D/Physics/RigidBody.h"
//#include "Urho3D/Physics/CollisionShape.h"
#include "rigid_body_2.h"
#include "collision_shape_2.h"

namespace Osp
{

Box::Box( Context * c )
    : Block( c, "Box" )
{
}

Box::~Box()
{

}

void Box::createContent( Node * node )
{
    Block::createContent( node );

    //n->CreateChild();
    StaticModel * obj = node->CreateComponent<StaticModel>();

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    obj->SetModel( cache->GetResource<Model>( "Models/Box.mdl" ) );
    obj->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
    obj->SetCastShadows( true );

    airMesh.init( obj );
}

void Box::toWorld()
{
    Block::toWorld();

    Node * node = GetNode();

    RigidBody2 * rb = node->CreateComponent<RigidBody2>();
    rb->SetMass( 1.0 );

    CollisionShape2 * cs = node->CreateComponent<CollisionShape2>();
    cs->SetBox( Vector3( 1.0, 1.0, 1.0 ) );
}

void Box::fromWorld()
{
    Block::fromWorld();
}



}


