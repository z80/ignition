
#ifndef __ATMOSPHERE_SOURCE_H_
#define __ATMOSPHERE_SOURCE_H_

#include "data_types.h"
#include "vector3d.h"
#include "ref_frame.h"
#include "air_mesh.h"

namespace Ign
{

class PhysicsItem;

class AtmosphereSource
{
public:
    AtmosphereSource();
    virtual ~AtmosphereSource();

    /// Atmosphere params at this distance from planet center.
    virtual bool params( Float distFromCenter, Float & pressure, Float & density ) const;
    /// Atmospheric drag force and torque.
    bool drag( AirMesh & a, const State & st, Vector3d & F, Vector3d & P ) const;

public:
    Float normalCoefP_,
          normalCoefN_,
          lateralCoef_;
};

}



#endif









