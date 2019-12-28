
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

}

bool TestEnvironment::ClientConnected( int id, const VariantMap & identity, String & errMsg )
{

    return true;
}


}





