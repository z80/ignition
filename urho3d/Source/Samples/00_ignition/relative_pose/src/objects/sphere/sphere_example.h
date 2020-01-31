
#ifndef __SPHERE_EXAMPLE_H_
#define __SPHERE_EXAMPLE_H_

#include "sphere_item.h"
#include "simple_source.h"

using namespace Urho3D;

namespace Ign
{

class SphereExample: public SphereItem
{
    URHO3D_OBJECT( SphereExample, SphereItem )
public:
    /// Register object factory.
    static void RegisterComponent( Context * context );

    SphereExample( Context * context );
    ~SphereExample();

protected:
    void applySourceCollision( Cubesphere & cs ) override;
    void applySourceVisual( Cubesphere & cs ) override;

    SimpleSource ss;
};



}



#endif






