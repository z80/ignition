

#ifndef __PLANET_MOON_TEST_H_
#define __PLANET_MOON_TEST_H_

#include "planet_base.h"

using namespace Urho3D;

namespace Osp
{

class PlanetMoonTest: public PlanetBase
{
    URHO3D_OBJECT( PlanetMoonTest, PlanetBase )
public:
    PlanetMoonTest( Context * ctx );
    ~PlanetMoonTest();

    void Start() override;
public:
    // Graphical objects
    SharedPtr<Node> sphereNode;
};

}

#endif



