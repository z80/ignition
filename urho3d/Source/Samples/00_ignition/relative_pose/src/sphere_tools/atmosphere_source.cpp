
#include "atmosphere_source.h"

namespace Ign
{

AtmosphereSource::AtmosphereSource()
{
}

AtmosphereSource::~AtmosphereSource()
{
}

bool AtmosphereSource::params( Float distFromCenter, Float & pressure, Float & density ) const
{
    pressure = 0.0;
    density  = 0.0;

    return false;
}

bool AtmosphereSource::drag( PhysicsItem * b, const State & st, Vector3d & F, Vector3d & P )
{

    return false;
}


}





