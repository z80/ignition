
#include "pi_atmosphere_source.h"

namespace Ign
{

PiAtmosphereSource::PiAtmosphereSource()
{
}

PiAtmosphereSource::PiAtmosphereSource( const PiSourceDesc & desc )
{
    desc_ = desc;
}

PiAtmosphereSource::~PiAtmosphereSource()
{
}

bool PiAtmosphereSource::params( Float distFromCenter, Float & pressure, Float & density ) const
{

    return false;
}


}




