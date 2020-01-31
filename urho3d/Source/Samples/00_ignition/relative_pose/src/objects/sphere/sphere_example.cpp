
#include "sphere_example.h"

namespace Ign
{

void SphereExample::RegisterComponent( Context * context )
{
    context->RegisterFactory<SphereExample>();
    URHO3D_COPY_BASE_ATTRIBUTES( SphereItem );
}

SphereExample::SphereExample( Context * context )
    : SphereItem( context )
{

}

SphereExample::~SphereExample()
{

}

void SphereExample::applySourceCollision( Cubesphere & cs )
{
    cs.applySource( &ss );
}

void SphereExample::applySourceVisual( Cubesphere & cs )
{
    cs.applySource( &ss );
}



}





