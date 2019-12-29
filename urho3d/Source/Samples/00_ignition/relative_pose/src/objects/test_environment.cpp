
#include "test_environment.h"
#include "static_mesh.h"
#include "infinite_plain.h"

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
    Scene * s = GetScene();
    StaticMesh * m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 10.0, 0.0, 0.0 ) );

    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 0.0, -10.0 ) );

    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 10.0, 0.0 ) );
    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 12.0, 0.0 ) );
}

bool TestEnvironment::ClientConnected( int id, const VariantMap & identity, String & errMsg )
{

    return true;
}


}





