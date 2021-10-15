
#ifndef __KEPLER_PLANET_MOVER_H_
#define __KEPLER_PLANET_MOVER_H_

#include "kepler_mover.h"
#include "planet_base.h"

namespace Osp
{

class KeplerMoverPlanet: public KeplerMover
{
    URHO3D_OBJECT( KeplerMoverPlanet, KeplerMover )
public:
    KeplerMoverPlanet( Context * ctx );
    ~KeplerMoverPlanet();

    void Start() override;
    void Update( float dt ) override;
public:
    SharedPtr<PlanetBase> planet;
};


}




#endif


