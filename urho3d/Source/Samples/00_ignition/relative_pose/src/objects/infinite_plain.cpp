
#include "infinite_plain.h"

namespace Ign
{

void InfinitePlain::RegisterComponent( Context * context )\
{
    context->RegisterFactory<InfinitePlain>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

InfinitePlain::InfinitePlain( Context * context )
    : RefFrame( context )
{

}

InfinitePlain::~InfinitePlain()
{

}


}

