
#include "planet_test.h"
#include "player.h"
#include "world_mover.h"
#include "physics_world_2.h"
#include "rigid_body_2.h"
#include "collision_shape_2.h"

namespace Osp
{

PlanetTest::PlanetTest( Context * ctx )
    : PlanetBase( ctx )
{

}

PlanetTest::~PlanetTest()
{

}

void PlanetTest::Start()
{
    PlanetBase::Start();

    // GM, a, e, Omega, I, omega, E
    mover->launch( 200.0, 50.0, 0.05, 0.0, 0.0 );
    rotator->launch( 3.0, 0.0 );
    mover->active = false;
    rotator->active = false;

    {
        // Testing drawing debug geometry.
        Node * n = GetNode();
        n->SetPosition( Vector3( 50.0, 0.0, 0.0 ) );
    }


    // Create graphical objects.
    {
        Random();
        // Sphere.
        sphereNode = SharedPtr<Node>( dynamicsNode->CreateChild( "SphereNode" ) );
        Node * s = sphereNode;
        StaticModel * m = s->CreateComponent<StaticModel>();

        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/PlanetTest.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        s->SetScale( ( 1.0, 1.0, 1.0 ) );
    }

    {
        // Launch site
        Node * n = SharedPtr<Node>( dynamicsNode->CreateChild( "LaunchSite" ) );
        //const Quaternion q = Quaternion( 70.0, Vector3( 0.0, 0.0, 1.0 ) );
        //const Vector3 at = q * Vector3( 0.0, 11.0, 0.0 );
        const Quaternion q = Quaternion( 0.0, Vector3( 0.0, 0.0, 1.0 ) );
        const Vector3 at = q * Vector3( 0.0, 11.0, 0.0 );

        n->SetPosition( at );
        n->SetRotation( q );
        site = n->CreateComponent<LaunchSite>();

        {
            Scene * s = GetScene();
            Camera * c = s->GetComponent( StringHash( "Camera" ), true )->Cast<Camera>();
            Node * cn = c->GetNode();
            cn->SetParent( n );
        }
    }

    // Create a player.
    {
        Node * n = site->GetNode();
        Node * playerNode = n->CreateChild( "PlayerNode" );
        playerNode->CreateComponent<Player>();
    }
}

void PlanetTest::updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
    Vector3d    rel_r;
    Quaterniond rel_q;
    rotator->relativePose( mover, rel_r, rel_q );

    Component * c = dynamicsNode->GetComponent( StringHash( "RigidBody2" ), true );
    if ( !c )
        return;
    RigidBody2 * rb  = c->Cast<RigidBody2>();
    if ( !rb )
        return;
    rb->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    rb->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );
}

void PlanetTest::initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
    Vector3d    rel_r;
    Quaterniond rel_q;
    rotator->relativePose( mover, rel_r, rel_q );

    Node * n0 = mover->GetNode();
    Node * n = n0->CreateChild( "PlanetSphere" );
    RigidBody2 * rb = n->CreateComponent<RigidBody2>();
    rb->SetMass( 0.0 );
    rb->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    rb->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );

    CollisionShape2 * cs = n->CreateComponent<CollisionShape2>();
    cs->SetSphere( 20.0 );
}

void PlanetTest::finitCollisions( Osp::PhysicsWorld2 * w2 )
{
    Component * c = dynamicsNode->GetComponent( StringHash( "RigidBody2" ), true );
    if ( !c )
        return;
    RigidBody2 * rb  = c->Cast<RigidBody2>();
    if ( !rb )
        return;
    Node * n = rb->GetNode();
    n->Remove();
}



}

