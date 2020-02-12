
#ifndef __PI_SYSTEM_GENERATOR_H_
#define __PI_SYSTEM_GEMERATOR_H_

#include "data_types.h"
#include "pi_system.h"
#include "pi_random.h"
#include "pi_source.h"

namespace Ign
{

class PiSystemGenerator
{
public:
    PiSystemGenerator();
    ~PiSystemGenerator();

    void apply( PiRandom & rand );

    void generateStars( PiRandom & rand );
    void makePlanetsAround( PiSystem & system, PiSourceDesc & primary, PiRandom & rand );
    void makeRandomStar( PiSourceDesc & sbody, PiRandom & rand );
    void makeStarOfType( PiSourceDesc & sbody, BodyType type, PiRandom & rand );
    void makeStarOfTypeLighterThan( PiSourceDesc & sbody, BodyType type, fixed maxMass, PiRandom & rand );

    void pickPlanetType( PiSourceDesc * sbody, PiRandom & rand );
    const PiSourceDesc * findStarAndTrueOrbitalRange( const PiSourceDesc * planet, fixed & orbMin_, fixed & orbMax_) const;
    int calcSurfaceTemp( PiSystem * system, const PiSourceDesc * primary, fixed distToPrimary, fixed albedo, fixed greenhouse );

    struct StarTypeInfo {
            int mass[2]; // min,max % sol for stars, unused for planets
            int radius[2]; // min,max % sol radii for stars, % earth radii for planets
            int tempMin, tempMax;
    };
    static const fixed starMetallicities[];
    static const StarTypeInfo starTypeInfo[];
};

}





#endif

