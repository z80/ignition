
#ifndef __PI_ATMOSPHERE_SOURCE_H_
#define __PI_ATMOSPHERE_SOURCE_H_

#include "pi_source.h"
#include "atmosphere_source.h"

namespace Ign
{

class PiAtmosphereSource: public AtmosphereSource
{
public:
    PiAtmosphereSource();
    PiAtmosphereSource( const PiSourceDesc & desc );
    ~PiAtmosphereSource();

    bool params( Float distFromCenter, Float & pressure, Float & density ) const override;

public:
    PiSourceDesc desc_;
};

}




#endif



