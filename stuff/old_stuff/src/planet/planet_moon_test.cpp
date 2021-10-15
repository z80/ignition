
#include "planet_moon_test.h"

namespace Osp
{

PlanetMoonTest::PlanetMoonTest( Context * ctx )
    : PlanetBase( ctx )
{

}

PlanetMoonTest::~PlanetMoonTest()
{

}

void PlanetMoonTest::Start()
{
    PlanetBase::Start();

    // GM, a, e, Omega, I, omega, E
    mover->launch( 100.0, 20.0, 0.05, 0.0, 0.0 );
    rotator->launch( 40, 0.0 );

    // Create graphical objects.
    {
        sphereNode = SharedPtr<Node>( dynamicsNode->CreateChild( "MoonNode" ) );
        Node * s = sphereNode;
        StaticModel * m = s->CreateComponent<StaticModel>();

        ResourceCache * cache = GetSubsystem<ResourceCache>();
        m->SetModel( cache->GetResource<Model>( "Models/MoonTest.mdl" ) );
        m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
        m->SetCastShadows( true );
        s->SetScale( ( 1.0, 1.0, 1.0 ) );
    }
}


}

