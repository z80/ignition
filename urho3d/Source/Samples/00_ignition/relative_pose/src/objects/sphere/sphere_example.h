
#ifndef __SPHERE_EXAMPLE_H_
#define __SPHERE_EXAMPLE_H_

#include "sphere_item.h"

using namespace Urho3D;

namespace Ign
{

class SphereExample: public SphereItem
{
    URHO3D_OBJECT( SphereExample, SphereItem )
public:
    SphereExample( Context * context );
    ~SphereExample
};



}



#endif






