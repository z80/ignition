
#include "test_environment.h"
#include "camera_frame.h"
#include "static_mesh.h"
#include "infinite_plane.h"
#include "dynamic_cube.h"
#include "ico_planet.h"

namespace Ign
{

void TestEnvironment::RegisterComponent( Context * context )
{
    context->RegisterFactory<TestEnvironment>();
}

TestEnvironment::TestEnvironment( Context * context )
    : Environment( context )
{

}

TestEnvironment::~TestEnvironment()
{

}

void TestEnvironment::DelayedStart()
{

}

bool TestEnvironment::ClientConnected( int id, const VariantMap & identity, String & errMsg )
{

    return true;
}

void TestEnvironment::CreateReplicatedContentServer()
{
    Scene * s = GetScene();

    InfinitePlane * p = s->CreateComponent<InfinitePlane>();
    p->setName( "Infinite plane object" );
    p->setR( Vector3d::ZERO );
    p->setQ( Quaterniond::IDENTITY );

    StaticMesh * m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 10.0, 0.0, 0.0 ) );

    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 0.0, -10.0 ) );

    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 10.0, 0.0 ) );
    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 12.0, 0.0 ) );

    IcoPlanet * ip = s->CreateComponent<IcoPlanet>();
    ip->setR( Vector3d( 0.0, 0.0, 20.0 ) );
}

void TestEnvironment::CreateReplicatedContentClient( CameraFrame * camera )
{
    Scene * s = GetScene();
    if ( !s )
        return;

    DynamicCube * d = s->CreateComponent<DynamicCube>();
    d->setName( String( "DynamicCube object" ) );
    d->setR( Vector3d( 0.0, 5.0, 0.0 ) );
    camera->setParent( d );
}


}





