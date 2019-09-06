
#include "planet_sun_test.h"

namespace Osp
{

PlanetSunTest::PlanetSunTest( Context * ctx )
    : PlanetBase( ctx )
{

}

PlanetSunTest::~PlanetSunTest()
{

}

void PlanetSunTest::Start()
{
    PlanetBase::Start();

    // GM, a, e, Omega, I, omega, E
    if ( mover )
        mover->Remove();
    if ( rotator )
        rotator->Remove();

    // Create graphical objects.
    {
        billboardNode = SharedPtr<Node>( dynamicsNode->CreateChild( "BillboardNode" ) );
        Node * s = billboardNode;
        BillboardSet * bbs = s->CreateComponent<BillboardSet>();
        bbs->SetNumBillboards( 1 );
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        Material * m = cache->GetResource<Material>( "Materials/SunTest.xml" );
        bbs->SetMaterial( m );

        Billboard * bb = bbs->GetBillboard( 0 );
        bb->position_  = Vector3::ZERO;
        bb->size_      = Vector2::ONE * 10.0;
        bb->rotation_  = 0.0;
        bb->enabled_   = true;

        bbs->Commit();
    }
}


}

