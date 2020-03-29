
#define NOMINMAX

#include "deterministic_system_generator.h"
#include "deterministic_source_0.h"


namespace Ign
{

static const fixed FIXED_PI = fixed(103993, 33102); // XXX Duplication from StarSystem.cpp

DeterministicSystemGenerator::DeterministicSystemGenerator()
{
}

DeterministicSystemGenerator::~DeterministicSystemGenerator()
{
}

void DeterministicSystemGenerator::generate_single_planet( PiSystem * system, PiRandom & rand )
{
    // periapsis, apoapsis = closest, farthest distance in orbit
    fixed pos( 1, 1 );
    fixed periapsis = pos + pos * fixed(1, 2) * rand.NFixed(2); /* + jump */

    fixed ecc = rand.NFixed(3);
    fixed semiMajorAxis = periapsis / (fixed(1, 1) - ecc);
    fixed apoapsis = 2 * semiMajorAxis - periapsis;

    //fixed mass = rand.Fixed();
    //mass += rand.Fixed();
    //mass += rand.Fixed();
    //mass += rand.Fixed();
    //mass += rand.Fixed();
    //mass /= 5;
    //const Float m = mass.ToDouble();
    const fixed mass = fixed( 1, 1 );


    PiSourceDesc planet;
    planet.eccentricity_ = ecc;
    planet.axial_tilt_ = fixed(100, 157) * rand.NFixed(2);
    planet.semimajor_axis_ = semiMajorAxis;
    planet.super_type_ = SUPERTYPE_ROCKY_PLANET;
    planet.type_ = TYPE_PLANET_TERRESTRIAL;
    planet.seed_ = rand.Int32();
    planet.parent_ind_ = -1;
    planet.mass_ = mass;
    //planet.rotation_period_ = fixed( rand.Int32(1, 200), 24 );
    planet.rotation_period_ = fixed( 6000, 86400 );
    planet.radius_ = fixed( 1, 1 );

    planet.gas_ = fixed( 50, 100 );
    planet.atmos_density_ = fixed( 100, 100 );

    const double e = ecc.ToDouble();

    const double r1 = rand.Double(2 * M_PI); // function parameter evaluation order is implementation-dependent
    const double r2 = rand.NDouble(5); // can't put two rands in the same expression
                                       //planet->m_orbit.SetPlane(matrix3x3d::RotateY(r1) * matrix3x3d::RotateX(-0.5 * M_PI + r2 * M_PI / 2.0));
                                       //planet->m_orbit.SetPhase(rand.Double(2 * M_PI));
    planet.X_ = r1;
    planet.Y_ = r2;


    planet.inclination_ = FIXED_PI;
    planet.inclination_ *= r2 / 2.0;
    planet.orb_min_ = periapsis;
    planet.orb_max_ = apoapsis;


    planet.super_type_ = SUPERTYPE_ROCKY_PLANET;
    planet.type_       = TYPE_PLANET_TERRESTRIAL;

    planet.metal_ = fixed( 6, 10 );


    // harder to be volcanic when you are tiny (you cool down)
    planet.volcanic_ = std::min( fixed(1, 1), planet.mass_ ) * rand.Fixed();
    planet.atm_oxidizing_ = rand.Fixed();
    planet.life_   = fixed( 5, 10 );
    planet.gas_    = fixed( 5, 10 );
    planet.liquid_ = fixed( 4, 10 );
    planet.ice_    = fixed( 2, 10 );
    planet.volcanic_ = fixed( 1, 10 );

    planet.average_temp_ = 278;

    system->bodies_.Clear();
    system->bodies_.Push( planet );
    system->root_body_ind_ = 0;
}

void DeterministicSystemGenerator::generate_system( PiSystem * system, PiRandom & rand )
{
    // To make it do something meaningful do the same thing here.
    generate_single_planet( system, rand );
}

HeightSource * DeterministicSystemGenerator::heightSource( const PiSourceDesc & bodyDesc )
{
    DeterministicSource0 * src = new DeterministicSource0( bodyDesc );
    return src;
}

}




