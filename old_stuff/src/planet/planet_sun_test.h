
#ifndef __PLANET_SUN_TEST_H_
#define __PLANET_SUN_TEST_H_

#include "planet_base.h"
#include "launch_site.h"

using namespace Urho3D;

namespace Osp
{

class PlanetSunTest: public PlanetBase
{
    URHO3D_OBJECT( PlanetSunTest, PlanetBase )
public:
    PlanetSunTest( Context * ctx );
    ~PlanetSunTest();

    void Start() override;
public:
    // Graphical objects
    SharedPtr<Node> billboardNode;
};

}

#endif



