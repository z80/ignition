
#ifndef __PLANET_FORCES_H_
#define __PLANET_FORCES_H_

#include <Urho3D/Urho3DAll.h>
#include "item_base.h"
#include "data_types.h"

namespace Osp
{

class Block;
class Assembly;

class PlanetForces: public LogicComponent
{
    URHO3D_OBJECT( PlanetForces, LogicComponent )
public:
    PlanetForces( Context * ctx );
    ~PlanetForces();

    virtual void Start() override;
    virtual void Update( float dt ) override;

    void applyClose( Block * b );
    void applyFar( Block * b );

    bool canOrbit( const ItemBase * a );

    bool load( const JSONValue & v );

public:
    virtual void applyGravity( Block * b );
    void clearGravity(Block * b );
    virtual void applyFriction( Block * b );

    bool  atmosphereParams( const Vector3d &at, Float & temperature, Float & densityF, Float & viscosityF,
                                                                     Float & densityB, Float & viscosityB );

    SharedPtr<ItemBase> planet;

    // Gravity parameter.
    Float GM_;
    // Gravity radius.
    Float R_;
    // Distance from center at which can orbit.
    // It means it is well beyond the atmosphere.
    Float orbitR_;

    // Air friction parameters.
    // Atmosphere height.
    Float atmHeight_;
    // Ground level density.
    Float densityF_, densityB_;
    // Viscosity.
    Float viscosityF_, viscosityB_;

    // Temperature parameters.
    Float groundT_;
    Float highAltitudeT_;

};

}

#endif


