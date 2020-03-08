
#ifndef __PI_ATMOSPHERE_SOURCE_H_
#define __PI_ATMOSPHERE_SOURCE_H_

#include "pi_source.h"
#include "atmosphere_source.h"

namespace Ign
{

/// In order to work properly this class MUST be assigned an 
/// instance of PiSourceDesc.
class PiAtmosphereSource: public AtmosphereSource
{
public:
    PiAtmosphereSource();
    PiAtmosphereSource( const PiSourceDesc & desc );
    ~PiAtmosphereSource();
    void operator=( const PiSourceDesc & desc );

    bool params( Float distFromCenter, Float & pressure, Float & density ) const override;

public:
    PiSourceDesc desc_;
    Float atmosphere_radius_,
          surface_gravity_;
};

}




#endif



