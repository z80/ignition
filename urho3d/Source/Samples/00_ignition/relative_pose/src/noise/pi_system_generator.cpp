
#define NOMINMAX


#include "pi_system_generator.h"
#include "pi_source.h"
#include "pi_utils.h"

#include <algorithm>

#define Square(x) (x*x)


namespace Ign
{

static const fixed SUN_MASS_TO_EARTH_MASS = fixed(332998, 1); // XXX Duplication from StarSystem.cpp
// if binary stars have separation s, planets can have stable
// orbits at (0.5 * s * SAFE_DIST_FROM_BINARY)
static const fixed SAFE_DIST_FROM_BINARY = fixed(5, 1);
// very crudely
static const fixed AU_SOL_RADIUS = fixed(305, 65536);
static const fixed AU_EARTH_RADIUS = fixed(3, 65536); // XXX Duplication from StarSystem.cpp
static const fixed FIXED_PI = fixed(103993, 33102); // XXX Duplication from StarSystem.cpp
static const double CELSIUS = 273.15;

static const Uint32 POLIT_SEED = 0x1234abcd;
static const Uint32 POLIT_SALT = 0x8732abdf;

const fixed PiSystemGenerator::starMetallicities[] = {
    fixed(1, 1), // GRAVPOINT - for planets that orbit them
    fixed(9, 10), // brown dwarf
    fixed(5, 10), // white dwarf
    fixed(7, 10), // M0
    fixed(6, 10), // K0
    fixed(5, 10), // G0
    fixed(4, 10), // F0
    fixed(3, 10), // A0
    fixed(2, 10), // B0
    fixed(1, 10), // O5
    fixed(8, 10), // M0 Giant
    fixed(65, 100), // K0 Giant
    fixed(55, 100), // G0 Giant
    fixed(4, 10), // F0 Giant
    fixed(3, 10), // A0 Giant
    fixed(2, 10), // B0 Giant
    fixed(1, 10), // O5 Giant
    fixed(9, 10), // M0 Super Giant
    fixed(7, 10), // K0 Super Giant
    fixed(6, 10), // G0 Super Giant
    fixed(4, 10), // F0 Super Giant
    fixed(3, 10), // A0 Super Giant
    fixed(2, 10), // B0 Super Giant
    fixed(1, 10), // O5 Super Giant
    fixed(1, 1), // M0 Hyper Giant
    fixed(7, 10), // K0 Hyper Giant
    fixed(6, 10), // G0 Hyper Giant
    fixed(4, 10), // F0 Hyper Giant
    fixed(3, 10), // A0 Hyper Giant
    fixed(2, 10), // B0 Hyper Giant
    fixed(1, 10), // O5 Hyper Giant
    fixed(1, 1), // M WF
    fixed(8, 10), // B WF
    fixed(6, 10), // O WF
    fixed(1, 1), //  S BH	Blackholes, give them high metallicity,
    fixed(1, 1), // IM BH	so any rocks that happen to be there
    fixed(1, 1) // SM BH	may be mining hotspots. FUN :)
};

const PiSystemGenerator::StarTypeInfo PiSystemGenerator::starTypeInfo[] = {
    { {}, {},
        0, 0 },
    { //Brown Dwarf
        { 2, 8 }, { 10, 30 },
        1000, 2000 },
    { //white dwarf
        { 20, 100 }, { 1, 2 },
        4000, 40000 },
    { //M
        { 10, 47 }, { 30, 60 },
        2000, 3500 },
    { //K
        { 50, 78 }, { 60, 100 },
        3500, 5000 },
    { //G
        { 80, 110 }, { 80, 120 },
        5000, 6000 },
    { //F
        { 115, 170 }, { 110, 150 },
        6000, 7500 },
    { //A
        { 180, 320 }, { 120, 220 },
        7500, 10000 },
    { //B
        { 200, 300 }, { 120, 290 },
        10000, 30000 },
    { //O
        { 300, 400 }, { 200, 310 },
        30000, 60000 },
    { //M Giant
        { 60, 357 }, { 2000, 5000 },
        2500, 3500 },
    { //K Giant
        { 125, 500 }, { 1500, 3000 },
        3500, 5000 },
    { //G Giant
        { 200, 800 }, { 1000, 2000 },
        5000, 6000 },
    { //F Giant
        { 250, 900 }, { 800, 1500 },
        6000, 7500 },
    { //A Giant
        { 400, 1000 }, { 600, 1000 },
        7500, 10000 },
    { //B Giant
        { 500, 1000 }, { 600, 1000 },
        10000, 30000 },
    { //O Giant
        { 600, 1200 }, { 600, 1000 },
        30000, 60000 },
    { //M Super Giant
        { 1050, 5000 }, { 7000, 15000 },
        2500, 3500 },
    { //K Super Giant
        { 1100, 5000 }, { 5000, 9000 },
        3500, 5000 },
    { //G Super Giant
        { 1200, 5000 }, { 4000, 8000 },
        5000, 6000 },
    { //F Super Giant
        { 1500, 6000 }, { 3500, 7000 },
        6000, 7500 },
    { //A Super Giant
        { 2000, 8000 }, { 3000, 6000 },
        7500, 10000 },
    { //B Super Giant
        { 3000, 9000 }, { 2500, 5000 },
        10000, 30000 },
    { //O Super Giant
        { 5000, 10000 }, { 2000, 4000 },
        30000, 60000 },
    { //M Hyper Giant
        { 5000, 15000 }, { 20000, 40000 },
        2500, 3500 },
    { //K Hyper Giant
        { 5000, 17000 }, { 17000, 25000 },
        3500, 5000 },
    { //G Hyper Giant
        { 5000, 18000 }, { 14000, 20000 },
        5000, 6000 },
    { //F Hyper Giant
        { 5000, 19000 }, { 12000, 17500 },
        6000, 7500 },
    { //A Hyper Giant
        { 5000, 20000 }, { 10000, 15000 },
        7500, 10000 },
    { //B Hyper Giant
        { 5000, 23000 }, { 6000, 10000 },
        10000, 30000 },
    { //O Hyper Giant
        { 10000, 30000 }, { 4000, 7000 },
        30000, 60000 },
    { // M WF
        { 2000, 5000 }, { 2500, 5000 },
        25000, 35000 },
    { // B WF
        { 2000, 7500 }, { 2500, 5000 },
        35000, 45000 },
    { // O WF
        { 2000, 10000 }, { 2500, 5000 },
        45000, 60000 },
    { // S BH
        { 20, 2000 }, { 0, 0 }, // XXX black holes are < 1 Sol radii big; this is clamped to a non-zero value later
        10, 24 },
    { // IM BH
        { 900000, 1000000 }, { 100, 500 },
        1, 10 },
    { // SM BH
        { 2000000, 5000000 }, { 10000, 20000 },
        10, 24 }
};



static fixed mass_from_disk_area( fixed a, fixed b, fixed max );
static fixed get_disc_density( const PiSourceDesc & primary, fixed discMin, fixed discMax, fixed percentOfPrimaryMass );
static inline bool test_overlap( const fixed & x1, const fixed & x2, const fixed & y1, const fixed & y2 );
static fixed calcEnergyPerUnitAreaAtDist( fixed star_radius, int star_temp, fixed object_dist );













PiSystemGenerator::PiSystemGenerator()
{

}

PiSystemGenerator::~PiSystemGenerator()
{

}

void PiSystemGenerator::createDumb( PiSystem * system, PiRandom & rand )
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
    const fixed mass = fixed( 1, 100000000 );


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
    planet.radius_ = fixed( 1, 640 );

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

    planet.metal_ = starMetallicities[ TYPE_STAR_G ] * rand.Fixed();


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

void PiSystemGenerator::apply( PiSystem * system, PiRandom & rand )
{
	BodyType starTypes[4];
    uint32_t seed = 0;
    for ( ;; )
    {
        rand.seed( seed );
        generateStars( system, starTypes, rand );
        if ( starTypes[0] == TYPE_STAR_G )
            break;
        seed += 1;
    }
    rand.seed( seed );
	const int numStars = generateStars( system, starTypes, rand );


	//PiSourceDesc * star[4];
	//PiSourceDesc * centGrav1(0), *centGrav2(0);

	assert((numStars >= 1) && ( numStars <= 4 ) );
	if (numStars == 1)
	{
		BodyType type = starTypes[0];;
        PiSourceDesc star;
		star.parent_ind_ = -1;
		star.orb_min_ = fixed();
		star.orb_max_ = fixed();

		makeStarOfType( star, type, rand );
		//system->SetRootBody( star[0] );
		//system->SetNumStars( 1 );
		system->root_body_ind_ = 0;

        // Add the star into the system.
        system->star_inds_.Push( system->bodies_.Size() );
        system->bodies_.Push( star );
	}
	/*
	else {
		centGrav1 = system->NewBody();
		centGrav1->m_type = SystemBody::TYPE_GRAVPOINT;
		centGrav1->m_parent = 0;
		centGrav1->m_name = sec->m_systems[system->GetPath().systemIndex].GetName() + " A,B";
		system->SetRootBody(centGrav1);

		SystemBody::BodyType type = sec->m_systems[system->GetPath().systemIndex].GetStarType(0);
		star[0] = system->NewBody();
		star[0]->m_name = sec->m_systems[system->GetPath().systemIndex].GetName() + " A";
		star[0]->m_parent = centGrav1;
		MakeStarOfType(star[0], type, rng);

		star[1] = system->NewBody();
		star[1]->m_name = sec->m_systems[system->GetPath().systemIndex].GetName() + " B";
		star[1]->m_parent = centGrav1;
		MakeStarOfTypeLighterThan(star[1], sec->m_systems[system->GetPath().systemIndex].GetStarType(1), star[0]->GetMassAsFixed(), rng);

		centGrav1->m_mass = star[0]->GetMassAsFixed() + star[1]->GetMassAsFixed();
		centGrav1->m_children.push_back(star[0]);
		centGrav1->m_children.push_back(star[1]);
		// Separate stars by 0.2 radii for each, so that their planets don't bump into the other star
		const fixed minDist1 = (fixed(12, 10) * star[0]->GetRadiusAsFixed() + fixed(12, 10) * star[1]->GetRadiusAsFixed()) * AU_SOL_RADIUS;
	try_that_again_guvnah:
		MakeBinaryPair(star[0], star[1], minDist1, rng);

		system->SetNumStars(2);

		if (numStars > 2) {
			if (star[0]->m_orbMax > fixed(100, 1)) {
				// reduce to < 100 AU...
				goto try_that_again_guvnah;
			}
			// 3rd and maybe 4th star
			if (numStars == 3) {
				star[2] = system->NewBody();
				star[2]->m_name = sec->m_systems[system->GetPath().systemIndex].GetName() + " C";
				star[2]->m_orbMin = 0;
				star[2]->m_orbMax = 0;
				MakeStarOfTypeLighterThan(star[2], sec->m_systems[system->GetPath().systemIndex].GetStarType(2), star[0]->GetMassAsFixed(), rng);
				centGrav2 = star[2];
				system->SetNumStars(3);
			} else {
				centGrav2 = system->NewBody();
				centGrav2->m_type = SystemBody::TYPE_GRAVPOINT;
				centGrav2->m_name = sec->m_systems[system->GetPath().systemIndex].GetName() + " C,D";
				centGrav2->m_orbMax = 0;

				star[2] = system->NewBody();
				star[2]->m_name = sec->m_systems[system->GetPath().systemIndex].GetName() + " C";
				star[2]->m_parent = centGrav2;
				MakeStarOfTypeLighterThan(star[2], sec->m_systems[system->GetPath().systemIndex].GetStarType(2), star[0]->GetMassAsFixed(), rng);

				star[3] = system->NewBody();
				star[3]->m_name = sec->m_systems[system->GetPath().systemIndex].GetName() + " D";
				star[3]->m_parent = centGrav2;
				MakeStarOfTypeLighterThan(star[3], sec->m_systems[system->GetPath().systemIndex].GetStarType(3), star[2]->GetMassAsFixed(), rng);

				// Separate stars by 0.2 radii for each, so that their planets don't bump into the other star
				const fixed minDist2 = (fixed(12, 10) * star[2]->GetRadiusAsFixed() + fixed(12, 10) * star[3]->GetRadiusAsFixed()) * AU_SOL_RADIUS;
				MakeBinaryPair(star[2], star[3], minDist2, rng);
				centGrav2->m_mass = star[2]->GetMassAsFixed() + star[3]->GetMassAsFixed();
				centGrav2->m_children.push_back(star[2]);
				centGrav2->m_children.push_back(star[3]);
				system->SetNumStars(4);
			}
			SystemBody *superCentGrav = system->NewBody();
			superCentGrav->m_type = SystemBody::TYPE_GRAVPOINT;
			superCentGrav->m_parent = 0;
			superCentGrav->m_name = sec->m_systems[system->GetPath().systemIndex].GetName();
			centGrav1->m_parent = superCentGrav;
			centGrav2->m_parent = superCentGrav;
			system->SetRootBody(superCentGrav);
			const fixed minDistSuper = star[0]->m_orbMax + star[2]->m_orbMax;
			MakeBinaryPair(centGrav1, centGrav2, 4 * minDistSuper, rng);
			superCentGrav->m_children.push_back(centGrav1);
			superCentGrav->m_children.push_back(centGrav2);
		}
	}*/

	// used in MakeShortDescription
	// XXX except this does not reflect the actual mining happening in this system
    PiSourceDesc & rootBody = system->bodies_[ system->root_body_ind_ ];
	rootBody.metal_ = starMetallicities[ rootBody.type_ ];

	// store all of the stars first ...
	//for (unsigned i = 0; i < system->GetNumStars(); i++ )
	//{
	//	system->stars_.Push( star[i] );
	//}
	// ... because we need them when making planets to calculate surface temperatures
	for ( auto starInd : system->star_inds_ )
	{
        PiSourceDesc primary = system->bodies_[starInd];
		makePlanetsAround( system, primary, system->root_body_ind_, rand );
        // Update it in the array;
        system->bodies_[starInd] = primary;
	}

	/*if (system->GetNumStars() > 1)
		MakePlanetsAround(system, centGrav1, rng);
	if (system->GetNumStars() == 4)
		MakePlanetsAround(system, centGrav2, rng);*/

	// an example export of generated system, can be removed during the merge
	//char filename[500];
	//snprintf(filename, 500, "tmp-sys/%s.lua", GetName().c_str());
	//ExportToLua(filename);

//#ifdef DEBUG_DUMP
//	Dump();
//#endif /* DEBUG_DUMP */
}

int PiSystemGenerator::generateStars( PiSystem * system, BodyType * starTypes, PiRandom & rand )
{
    PiSystem & s = *system;


    const int sx = s.sx;
    const int sy = s.sy;
    const int sz = s.sz;
    const Sint64 dist = (1 + sx * sx + sy * sy + sz * sz);
    const Sint64 freq = (1 + sx * sx + sy * sy);

	int starsQty = 0;


    const int qty = rand.Int32(15);
    switch ( qty )
    {
    case 0:
	    starsQty = 4;
        break;
    case 1:
    case 2:
	    starsQty = 3;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
	    starsQty = 2;
        break;
    default:
	    starsQty = 1;
        break;
    }


	// At the moment enforce number of start to be 1.
	// I didn't port rotation around barycenter yet.
    starsQty = 1;


    if ( freq > Square(10) )
    {
        const Uint32 weight = rand.Int32(1000000);
        if (weight < 1) {
		    starTypes[0] = TYPE_STAR_IM_BH; // These frequencies are made up
        } else if (weight < 3) {
			starTypes[0] = TYPE_STAR_S_BH;
        } else if (weight < 5) {
            starTypes[0] = TYPE_STAR_O_WF;
        } else if (weight < 8) {
            starTypes[0] = TYPE_STAR_B_WF;
        } else if (weight < 12) {
            starTypes[0] = TYPE_STAR_M_WF;
        } else if (weight < 15) {
            starTypes[0] = TYPE_STAR_K_HYPER_GIANT;
        } else if (weight < 18) {
            starTypes[0] = TYPE_STAR_G_HYPER_GIANT;
        } else if (weight < 23) {
            starTypes[0] = TYPE_STAR_O_HYPER_GIANT;
        } else if (weight < 28) {
            starTypes[0] = TYPE_STAR_A_HYPER_GIANT;
        } else if (weight < 33) {
            starTypes[0] = TYPE_STAR_F_HYPER_GIANT;
        } else if (weight < 41) {
            starTypes[0] = TYPE_STAR_B_HYPER_GIANT;
        } else if (weight < 48) {
            starTypes[0] = TYPE_STAR_M_HYPER_GIANT;
        } else if (weight < 58) {
            starTypes[0] = TYPE_STAR_K_SUPER_GIANT;
        } else if (weight < 68) {
            starTypes[0] = TYPE_STAR_G_SUPER_GIANT;
        } else if (weight < 78) {
            starTypes[0] = TYPE_STAR_O_SUPER_GIANT;
        } else if (weight < 88) {
            starTypes[0] = TYPE_STAR_A_SUPER_GIANT;
        } else if (weight < 98) {
            starTypes[0] = TYPE_STAR_F_SUPER_GIANT;
        } else if (weight < 108) {
            starTypes[0] = TYPE_STAR_B_SUPER_GIANT;
        } else if (weight < 158) {
            starTypes[0] = TYPE_STAR_M_SUPER_GIANT;
        } else if (weight < 208) {
            starTypes[0] = TYPE_STAR_K_GIANT;
        } else if (weight < 250) {
            starTypes[0] = TYPE_STAR_G_GIANT;
        } else if (weight < 300) {
            starTypes[0] = TYPE_STAR_O_GIANT;
        } else if (weight < 350) {
            starTypes[0] = TYPE_STAR_A_GIANT;
        } else if (weight < 400) {
            starTypes[0] = TYPE_STAR_F_GIANT;
        } else if (weight < 500) {
            starTypes[0] = TYPE_STAR_B_GIANT;
        } else if (weight < 700) {
            starTypes[0] = TYPE_STAR_M_GIANT;
        } else if (weight < 800) {
            starTypes[0] = TYPE_STAR_O; // should be 1 but that is boring
        } else if (weight < 2000) { // weight < 1300 / 20500
            starTypes[0] = TYPE_STAR_B;
        } else if (weight < 8000) { // weight < 7300
            starTypes[0] = TYPE_STAR_A;
        } else if (weight < 37300) { // weight < 37300
            starTypes[0] = TYPE_STAR_F;
        } else if (weight < 113300) { // weight < 113300
            starTypes[0] = TYPE_STAR_G;
        } else if (weight < 234300) { // weight < 234300
            starTypes[0] = TYPE_STAR_K;
        } else if (weight < 250000) { // weight < 250000
            starTypes[0] = TYPE_WHITE_DWARF;
        } else if (weight < 900000) { //weight < 900000
            starTypes[0] = TYPE_STAR_M;
        } else {
            starTypes[0] = TYPE_BROWN_DWARF;
        }
    } else {
        const Uint32 weight = rand.Int32(1000000);
        if (weight < 100) { // should be 1 but that is boring
            starTypes[0] = TYPE_STAR_O;
        } else if (weight < 1300) {
            starTypes[0] = TYPE_STAR_B;
        } else if (weight < 7300) {
            starTypes[0] = TYPE_STAR_A;
        } else if (weight < 37300) {
            starTypes[0] = TYPE_STAR_F;
        } else if (weight < 113300) {
            starTypes[0] = TYPE_STAR_G;
        } else if (weight < 234300) {
            starTypes[0] = TYPE_STAR_K;
        } else if (weight < 250000) {
            starTypes[0] = TYPE_WHITE_DWARF;
        } else if (weight < 900000) {
            starTypes[0] = TYPE_STAR_M;
        } else {
            starTypes[0] = TYPE_BROWN_DWARF;
        }
    }
    //Output("%d: %d%\n", sx, sy);

    if ( starsQty > 1)
    {
        starTypes[1] = BodyType( rand.Int32( TYPE_STAR_MIN, starTypes[0] ) );
        if ( starsQty > 2 )
        {
            starTypes[2] = BodyType( rand.Int32( TYPE_STAR_MIN, starTypes[0] ) );
            starTypes[3] = BodyType( rand.Int32( TYPE_STAR_MIN, starTypes[2] ) );
        }
    }

	const Uint32 rand10 = rand.Int32( 10 );
    if ( ( starTypes[0] <= TYPE_STAR_A ) && ( rand10 == 0 ) )
    {
        // make primary a giant. never more than one giant in a system
        if (freq > Square(10))
        {
            const Uint32 weight = rand.Int32(1000);
            if (weight >= 999) {
                starTypes[0] = TYPE_STAR_B_HYPER_GIANT;
            } else if (weight >= 998) {
                starTypes[0] = TYPE_STAR_O_HYPER_GIANT;
            } else if (weight >= 997) {
                starTypes[0] = TYPE_STAR_K_HYPER_GIANT;
            } else if (weight >= 995) {
                starTypes[0] = TYPE_STAR_B_SUPER_GIANT;
            } else if (weight >= 993) {
                starTypes[0] = TYPE_STAR_O_SUPER_GIANT;
            } else if (weight >= 990) {
                starTypes[0] = TYPE_STAR_K_SUPER_GIANT;
            } else if (weight >= 985) {
                starTypes[0] = TYPE_STAR_B_GIANT;
            } else if (weight >= 980) {
                starTypes[0] = TYPE_STAR_O_GIANT;
            } else if (weight >= 975) {
                starTypes[0] = TYPE_STAR_K_GIANT;
            } else if (weight >= 950) {
                starTypes[0] = TYPE_STAR_M_HYPER_GIANT;
            } else if (weight >= 875) {
                starTypes[0] = TYPE_STAR_M_SUPER_GIANT;
            } else {
                starTypes[0] = TYPE_STAR_M_GIANT;
            }
        } else if (freq > Square(5))
            starTypes[0] = TYPE_STAR_M_GIANT;
        else
            starTypes[0] = TYPE_STAR_M;

        //Output("%d: %d%\n", sx, sy);
    }

    //s.m_name = GenName(galaxy, *sector, s, customCount + i, rng);
    //Output("%s: \n", s.m_name.c_str());

    //s.m_systems.push_back(s);

	return starsQty;
}

void PiSystemGenerator::makePlanetsAround( PiSystem * system, PiSourceDesc & primary, int primaryInd, PiRandom & rand )
{
    fixed discMin = fixed();
    fixed discMax = fixed(5000, 1);
    fixed discDensity;

    BodySuperType parentSuperType = primary.super_type_;

    if (parentSuperType <= SUPERTYPE_STAR)
    {
        if ( primary.type_ == TYPE_GRAVPOINT )
        {
            /* around a binary */
            discMin = system->bodies_[ primary.child_inds_[0] ].orb_min_ * SAFE_DIST_FROM_BINARY;
        } else {
            /* correct thing is roche limit, but lets ignore that because
             * it depends on body densities and gives some strange results */
            discMin = 4 * primary.radius_ * AU_SOL_RADIUS;
        }
        if (primary.type_ == TYPE_WHITE_DWARF)
        {
            // white dwarfs will have started as stars < 8 solar
            // masses or so, so pick discMax according to that
            // We give it a larger discMin because it used to be a much larger star
            discMin = 1000 * primary.radius_ * AU_SOL_RADIUS;
            discMax = 100 * rand.NFixed(2); // rand-splitting again
            discMax *= fixed::SqrtOf(fixed(1, 2) + fixed(8, 1) * rand.Fixed());
        }
        else
        {
            discMax = 100 * rand.NFixed(2) * fixed::SqrtOf(primary.mass_);
        }
        // having limited discMin by bin-separation/fake roche, and
        // discMax by some relation to star mass, we can now compute
        // disc density
        discDensity = rand.Fixed() * get_disc_density( primary, discMin, discMax, fixed(2, 100) );

        if ( (parentSuperType == SUPERTYPE_STAR) && ( primary.parent_ind_ >= 0 ) )
        {
            // limit planets out to 10% distance to star's binary companion
            discMax = std::min( discMax, primary.orb_min_ * fixed(1, 10) );
        }

        /* in trinary and quaternary systems don't bump into other pair... */
        if ( system->GetNumStars() >= 3 )
        {
            const PiSourceDesc & rootBody = system->bodies_[ system->root_body_ind_ ];
			const PiSourceDesc & firstChild = system->bodies_[ rootBody.child_inds_[0] ];
            discMax = std::min( discMax, fixed(5, 100) * firstChild.orb_min_ );
        }
    }
    else
    {
        fixed primary_rad = primary.radius_ * AU_EARTH_RADIUS;
        discMin = 4 * primary_rad;
        /* use hill radius to find max size of moon system. for stars botch it.
           And use planets orbit around its primary as a scaler to a moon's orbit*/
        discMax = std::min( discMax, fixed(1, 20) * calcHillRadius( system, primary ) * primary.orb_min_ * fixed(1, 10) );

        discDensity = rand.Fixed() * get_disc_density( primary, discMin, discMax, fixed(1, 500) );
    }

    //fixed discDensity = 20*rand.NFixed(4);

    //Output("Around %s: Range %f -> %f AU\n", primary.GetName().c_str(), discMin.ToDouble(), discMax.ToDouble());

    fixed initialJump = rand.NFixed(5);
    fixed pos = (fixed(1, 1) - initialJump) * discMin + (initialJump * discMax);
    //const RingStyle & ring = primary.GetRings();
    //const bool hasRings = primary.HasRings();

    while (pos < discMax)
    {
        // periapsis, apoapsis = closest, farthest distance in orbit
        fixed periapsis = pos + pos * fixed(1, 2) * rand.NFixed(2); /* + jump */

        fixed ecc = rand.NFixed(3);
        fixed semiMajorAxis = periapsis / (fixed(1, 1) - ecc);
        fixed apoapsis = 2 * semiMajorAxis - periapsis;
        if (apoapsis > discMax) break;

        fixed mass;
        {
            const fixed a = pos;
            const fixed b = fixed(135, 100) * apoapsis;
            mass = mass_from_disk_area( a, b, discMax );
            mass *= rand.Fixed() * discDensity;
        }
        if (mass < 0)
        {
            // hack around overflow
            //Output("WARNING: planetary mass has overflowed! (child of %s)\n", primary.GetName().c_str());
            mass = fixed(Sint64(0x7fFFffFFffFFffFFull));
        }
        assert(mass >= 0);

        PiSourceDesc planet;
        planet.eccentricity_ = ecc;
        planet.axial_tilt_ = fixed(100, 157) * rand.NFixed(2);
        planet.semimajor_axis_ = semiMajorAxis;
		planet.super_type_ = SUPERTYPE_ROCKY_PLANET;
        planet.type_ = TYPE_PLANET_TERRESTRIAL;
        planet.seed_ = rand.Int32();
        planet.parent_ind_ = primaryInd;
        planet.mass_ = mass;
        planet.rotation_period_ = fixed( rand.Int32(1, 200), 24 );

        const double e = ecc.ToDouble();

        //if ( primary.type_ == TYPE_GRAVPOINT )
        //    planet->m_orbit.SetShapeAroundBarycentre( semiMajorAxis.ToDouble() * AU, primary.GM_, planet->GM_, e );
        //else
        //    planet->m_orbit.SetShapeAroundPrimary(semiMajorAxis.ToDouble() * AU, primary.GetMass(), e);

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
        primary.child_inds_.Push( system->bodies_.Size() );
        system->bodies_.Push( planet );

        //if (hasRings &&
        //    parentSuperType == SystemBody::SUPERTYPE_ROCKY_PLANET &&
        //    test_overlap(ring.minRadius, ring.maxRadius, periapsis, apoapsis)) {
        //    //Output("Overlap, eliminating rings from parent SystemBody\n");
        //    //Overlap, eliminating rings from parent SystemBody
        //    primary.m_rings.minRadius = fixed();
        //    primary.m_rings.maxRadius = fixed();
        //    primary.m_rings.baseColor = Color(255, 255, 255, 255);
        //}

        /* minimum separation between planets of 1.35 */
        pos = apoapsis * fixed( 135, 100 );
    }

    int idx = 0;
    bool make_moons = parentSuperType <= SUPERTYPE_STAR;

    for ( Vector<int>::Iterator i = primary.child_inds_.Begin(); i != primary.child_inds_.End(); ++i)
    {
        const int childInd = *i;
        PiSourceDesc child = system->bodies_[ childInd ];
        // planets around a binary pair [gravpoint] -- ignore the stars...
        if ( child.super_type_ == SUPERTYPE_STAR)
            continue;
        // Turn them into something!!!!!!!
        char buf[12];
        if ( parentSuperType <= SUPERTYPE_STAR )
        {
            // planet naming scheme
            snprintf(buf, sizeof(buf), " %c", 'a' + idx);
        }
		else
		{
            // moon naming scheme
            snprintf(buf, sizeof(buf), " %d", 1 + idx);
        }
        //(*i)->m_name = primary.GetName() + buf;
        pickPlanetType( system, child, rand );
        if ( make_moons )
            makePlanetsAround( system, child, childInd, rand );

        // Update current child in the system.
        system->bodies_[ childInd ] = child;
        idx++;
    }
}

void PiSystemGenerator::makeRandomStar( PiSourceDesc & sbody, PiRandom & rand )
{
	BodyType type = BodyType( rand.Int32( TYPE_STAR_MIN, TYPE_STAR_MAX ) );
	makeStarOfType( sbody, type, rand );
}

void PiSystemGenerator::makeStarOfType( PiSourceDesc & sbody, BodyType type, PiRandom & rand )
{
	sbody.super_type_ = SUPERTYPE_STAR;
	sbody.type_ = type;
	sbody.seed_ = rand.Int32();
	sbody.radius_ = fixed( rand.Int32( starTypeInfo[type].radius[0], starTypeInfo[type].radius[1] ), 100 );

	// Assign aspect ratios caused by equatorial bulges due to rotation. See terrain code for details.
	// XXX to do: determine aspect ratio distributions for dimmer stars. Make aspect ratios consistent with rotation speeds/stability restrictions.
	switch ( type )
	{
		// Assign aspect ratios (roughly) between 1.0 to 1.8 with a bias towards 1 for bright stars F, A, B ,O

		// "A large fraction of hot stars are rapid rotators with surface rotational velocities
		// of more than 100 km/s (6, 7). ." Imaging the Surface of Altair, John D. Monnier, et. al. 2007
		// A reasonable amount of lot of stars will be assigned high aspect ratios.

		// Bright stars whose equatorial to polar radius ratio (the aspect ratio) is known
		// seem to tend to have values between 1.0 and around 1.5 (brief survey).
		// The limiting factor preventing much higher values seems to be stability as they
		// are rotating 80-95% of their breakup velocity.
	case TYPE_STAR_F:
	case TYPE_STAR_F_GIANT:
	case TYPE_STAR_F_HYPER_GIANT:
	case TYPE_STAR_F_SUPER_GIANT:
	case TYPE_STAR_A:
	case TYPE_STAR_A_GIANT:
	case TYPE_STAR_A_HYPER_GIANT:
	case TYPE_STAR_A_SUPER_GIANT:
	case TYPE_STAR_B:
	case TYPE_STAR_B_GIANT:
	case TYPE_STAR_B_SUPER_GIANT:
	case TYPE_STAR_B_WF:
	case TYPE_STAR_O:
	case TYPE_STAR_O_GIANT:
	case TYPE_STAR_O_HYPER_GIANT:
	case TYPE_STAR_O_SUPER_GIANT:
	case TYPE_STAR_O_WF:
	{
		fixed rnd = rand.Fixed();
		sbody.aspectRatio_ = fixed(1, 1) + fixed(8, 10) * rnd * rnd;
		break;
	}
	// aspect ratio is initialised to 1.0 for other stars currently
	default:
        sbody.aspectRatio_ = fixed(1, 1);
		break;
	}
	sbody.mass_ = fixed( rand.Int32( starTypeInfo[type].mass[0], starTypeInfo[type].mass[1] ), 100 );
	sbody.average_temp_ = rand.Int32( starTypeInfo[type].tempMin, starTypeInfo[type].tempMax );
}

void PiSystemGenerator::makeStarOfTypeLighterThan( PiSourceDesc & sbody, BodyType type, fixed maxMass, PiRandom & rand )
{
	int tries = 16;
	do {
		makeStarOfType( sbody, type, rand );
	} while ( ( sbody.mass_ > maxMass ) && (--tries) );
}

/*
* http://en.wikipedia.org/wiki/Hill_sphere
*/
fixed PiSystemGenerator::calcHillRadius( PiSystem * system, const PiSourceDesc & sbody ) const
{
		if (sbody.super_type_ <= SUPERTYPE_STAR)
		{
			return fixed();
		}
		else
		{
			// playing with precision since these numbers get small
			// masses in earth masses
			const PiSourceDesc & parent = system->bodies_[ sbody.parent_ind_ ];
			fixedf<32> mprimary = parent.mass_;

			fixedf<48> a = sbody.semimajor_axis_;
			fixedf<48> e = sbody.eccentricity_;

			return fixed(a * (fixedf<48>(1, 1) - e) *
				fixedf<48>::CubeRootOf(fixedf<48>(
					sbody.mass_ / (fixedf<32>(3, 1) * mprimary))));

			//fixed hr = semiMajorAxis*(fixed(1,1) - eccentricity) *
			//  fixedcuberoot(mass / (3*mprimary));
		}
}














void PiSystemGenerator::pickPlanetType( PiSystem * system, PiSourceDesc & sbody, PiRandom & rand )
{
    fixed albedo;
    fixed greenhouse;

    fixed minDistToStar, maxDistToStar, averageDistToStar;
    const PiSourceDesc * star = findStarAndTrueOrbitalRange( system, sbody, minDistToStar, maxDistToStar );
    averageDistToStar = (minDistToStar + maxDistToStar) >> 1;

    /* first calculate blackbody temp (no greenhouse effect, zero albedo) */
    const int bbody_temp = calcSurfaceTemp( system, *star, averageDistToStar, albedo, greenhouse );

    sbody.average_temp_ = bbody_temp;

    static const fixed ONEEUMASS = fixed::FromDouble(1);
    static const fixed TWOHUNDREDEUMASSES = fixed::FromDouble(200.0);
    // We get some more fractional bits for small bodies otherwise we can easily end up with 0 radius which breaks stuff elsewhere
    //
    // AndyC - Updated to use the empirically gathered data from this site:
    // http://phl.upr.edu/library/notes/standardmass-radiusrelationforexoplanets
    // but we still limit at the lowest end
    if (sbody.mass_ <= fixed(1, 1))
	{
        sbody.radius_ = fixed( fixedf<48>::CubeRootOf( fixedf<48>( sbody.mass_ ) ) );
    }
	else if ( sbody.mass_ < ONEEUMASS )
	{
        // smaller than 1 Earth mass is almost certainly a rocky body
        sbody.radius_ = fixed::FromDouble( pow( sbody.mass_.ToDouble(), 0.3 ) );
    }
	else if ( sbody.mass_ < TWOHUNDREDEUMASSES )
	{
        // from 1 EU to 200 they transition from Earth-like rocky bodies, through Ocean worlds and on to Gas Giants
        sbody.radius_ = fixed::FromDouble( pow( sbody.mass_.ToDouble(), 0.5 ) );
    }
	else
	{
        // Anything bigger than 200 EU masses is a Gas Giant or bigger but the density changes to decrease from here on up...
        sbody.radius_ = fixed::FromDouble( 22.6 * ( 1.0 / pow( sbody.mass_.ToDouble(), double(0.0886) ) ) );
    }
    // enforce minimum size of 10km
    sbody.radius_ = std::max( sbody.radius_, fixed( 1, 630 ) );

	const PiSourceDesc & parent = system->bodies_[ sbody.parent_ind_ ];
    if ( parent.type_ <= TYPE_STAR_MAX )
	{
        // get it from the table now rather than setting it on stars/gravpoints as
        // currently nothing else needs them to have metallicity
        sbody.metal_ = starMetallicities[ parent.type_ ] * rand.Fixed();
    }
	else
	{
        // this assumes the parent's parent is a star/gravpoint, which is currently always true
		const PiSourceDesc & parentParent = system->bodies_[ parent.parent_ind_ ];
        sbody.metal_ = starMetallicities[ parentParent.type_ ] * rand.Fixed();
    }

    // harder to be volcanic when you are tiny (you cool down)
    sbody.volcanic_ = std::min( fixed(1, 1), sbody.mass_ ) * rand.Fixed();
    sbody.atm_oxidizing_ = rand.Fixed();
    sbody.life_   = fixed();
    sbody.gas_    = fixed();
    sbody.liquid_ = fixed();
    sbody.ice_    = fixed();

    // pick body type
    if (sbody.mass_ > 317 * 13)
	{
        // more than 13 jupiter masses can fuse deuterium - is a brown dwarf
        sbody.type_ = TYPE_BROWN_DWARF;
        sbody.average_temp_ = sbody.average_temp_ + rand.Int32( starTypeInfo[ sbody.type_ ].tempMin, starTypeInfo[ sbody.type_ ].tempMax );
        // prevent mass exceeding 65 jupiter masses or so, when it becomes a star
        // XXX since TYPE_BROWN_DWARF is supertype star, mass is now in
        // solar masses. what a fucking mess
        sbody.mass_ = std::min( sbody.mass_, fixed(317 * 65, 1) ) / SUN_MASS_TO_EARTH_MASS;
        //Radius is too high as it now uses the planetary calculations to work out radius (Cube root of mass)
        // So tell it to use the star data instead:
        sbody.radius_ = fixed( rand.Int32( starTypeInfo[ sbody.type_ ].radius[0], 
							starTypeInfo[ sbody.type_ ].radius[1] ), 100 );
    }
	else if ( sbody.mass_ > 6 )
	{
        sbody.type_ = TYPE_PLANET_GAS_GIANT;
    }
	else if ( sbody.mass_ > fixed(1, 15000) )
	{
        sbody.type_ = TYPE_PLANET_TERRESTRIAL;

        fixed amount_volatiles = fixed(2, 1) * rand.Fixed();
        if ( rand.Int32(3) )
			amount_volatiles *= sbody.mass_;
        // total atmosphere loss
        if ( rand.Fixed() > sbody.mass_)
			amount_volatiles = fixed();

        //Output("Amount volatiles: %f\n", amount_volatiles.ToFloat());
        // fudge how much of the volatiles are in which state
        greenhouse = fixed();
        albedo = fixed();
        // CO2 sublimation
        if ( sbody.average_temp_ > 195 )
            greenhouse += amount_volatiles * fixed(1, 3);
        else
            albedo += fixed(2, 6);
        // H2O liquid
        if ( sbody.average_temp_ > 273 )
            greenhouse += amount_volatiles * fixed(1, 5);
        else
            albedo += fixed(3, 6);
        // H2O boils
        if ( sbody.average_temp_ > 373)
			greenhouse += amount_volatiles * fixed(1, 3);

        if ( greenhouse > fixed(7, 10) )
		{ 
			 // never reach 1, but 1/(1-greenhouse) still grows
             greenhouse *= greenhouse;
             greenhouse *= greenhouse;
             greenhouse = greenhouse / (greenhouse + fixed(32, 311));
        }

        sbody.average_temp_ = calcSurfaceTemp( system, *star, averageDistToStar, albedo, greenhouse );

        const fixed proportion_gas = sbody.average_temp_ / ( fixed(100, 1) + sbody.average_temp_ );
        sbody.gas_ = proportion_gas * amount_volatiles;

        const fixed proportion_liquid = ( fixed(1, 1) - proportion_gas ) * (sbody.average_temp_ / ( fixed(50, 1) + sbody.average_temp_ ) );
        sbody.liquid_ = proportion_liquid * amount_volatiles;

        const fixed proportion_ices = fixed(1, 1) - (proportion_gas + proportion_liquid);
        sbody.ice_ = proportion_ices * amount_volatiles;

        //Output("temp %dK, gas:liquid:ices %f:%f:%f\n", averageTemp, proportion_gas.ToFloat(),
        //		proportion_liquid.ToFloat(), proportion_ices.ToFloat());

        if ( (sbody.liquid_ > fixed()) &&
             (sbody.average_temp_ > CELSIUS - 60) &&
             (sbody.average_temp_ < CELSIUS + 200))
		{
            // try for life
            int minTemp = calcSurfaceTemp( system, *star, maxDistToStar, albedo, greenhouse);
            int maxTemp = calcSurfaceTemp( system, *star, minDistToStar, albedo, greenhouse);

            if ((minTemp > CELSIUS - 10) && (minTemp < CELSIUS + 90) && //removed explicit checks for star type (also BD and WD seem to have slight chance of having life around them)
                (maxTemp > CELSIUS - 10) && (maxTemp < CELSIUS + 90)) //TODO: ceiling based on actual boiling point on the planet, not in 1atm
            {
                fixed maxMass, lifeMult, allowedMass(1, 2);
                allowedMass += 2;
                for ( auto s : system->star_inds_ )
				{ 
					const PiSourceDesc & star = system->bodies_[ s ];
					//find the most massive star, mass is tied to lifespan
                    maxMass = maxMass < star.mass_ ? star.mass_ : maxMass; //this automagically eliminates O, B and so on from consideration
                } //handy calculator: http://www.asc-csa.gc.ca/eng/educators/resources/astronomy/module2/calculator.asp
                if ( maxMass < allowedMass )
				{
					//system could have existed long enough for life to form (based on Sol)
                    lifeMult = allowedMass - maxMass;
                }
                sbody.life_ = lifeMult * rand.Fixed();
            }
        }
    }
	else
	{
        sbody.type_ = TYPE_PLANET_ASTEROID;
    }

    // Tidal lock for planets close to their parents:
    //		http://en.wikipedia.org/wiki/Tidal_locking
    //
    //		Formula: time ~ semiMajorAxis^6 * radius / mass / parentMass^2
    //
    //		compared to Earth's Moon
    static fixed MOON_TIDAL_LOCK = fixed(6286, 1);
    fixed invTidalLockTime = fixed(1, 1);

    // fine-tuned not to give overflows, order of evaluation matters!
    if ( parent.type_ <= TYPE_STAR_MAX )
	{
        invTidalLockTime /= (sbody.semimajor_axis_ * sbody.semimajor_axis_);
        invTidalLockTime *= sbody.mass_;
        invTidalLockTime /= (sbody.semimajor_axis_ * sbody.semimajor_axis_);
        invTidalLockTime *= parent.mass_ * parent.mass_;
        invTidalLockTime /= sbody.radius_;
        invTidalLockTime /= (sbody.semimajor_axis_ * sbody.semimajor_axis_) * MOON_TIDAL_LOCK;
    } else {
        invTidalLockTime /= (sbody.semimajor_axis_ * sbody.semimajor_axis_) * SUN_MASS_TO_EARTH_MASS;
        invTidalLockTime *= sbody.mass_;
        invTidalLockTime /= (sbody.semimajor_axis_ * sbody.semimajor_axis_) * SUN_MASS_TO_EARTH_MASS;
        invTidalLockTime *= parent.mass_ * parent.mass_;
        invTidalLockTime /= sbody.radius_;
        invTidalLockTime /= (sbody.semimajor_axis_ * sbody.semimajor_axis_) * MOON_TIDAL_LOCK;
    }
    //Output("tidal lock of %s: %.5f, a %.5f R %.4f mp %.3f ms %.3f\n", name.c_str(),
    //		invTidalLockTime.ToFloat(), semiMajorAxis.ToFloat(), radius.ToFloat(), parent->mass.ToFloat(), mass.ToFloat());


	/*
    if (invTidalLockTime > 10) { // 10x faster than Moon, no chance not to be tidal-locked
        sbody.rotation_period_ = fixed( int( round( sbody.GetOrbit().Period() ) ), 3600 * 24 );
        sbody.axial_tilt_ = sbody.inclination_;
    } else if ( invTidalLockTime > fixed(1, 100) )
	{   
		// rotation speed changed in favour of tidal lock
        // XXX: there should be some chance the satellite was captured only recenly and ignore this
        //		I'm ommiting that now, I do not want to change the Universe by additional rand call.

        fixed lambda = invTidalLockTime / (fixed(1, 20) + invTidalLockTime);
        sbody.rotation_period_ = (1 - lambda) * sbody.rotation_period_ + lambda * sbody.GetOrbit().Period() / 3600 / 24;
        sbody.axial_tilt_ = (1 - lambda) * sbody.axial_tilt_ + lambda * sbody.inclination_;
    } // else .. nothing happens to the satellite
	*/

    pickAtmosphere( sbody );
    //PickRings(sbody);
}

const PiSourceDesc * PiSystemGenerator::findStarAndTrueOrbitalRange( PiSystem * system, const PiSourceDesc & planetOrigin, fixed & orbMin_, fixed & orbMax_) const
{
    const PiSourceDesc * planet = &planetOrigin;
    int starInd = planet->parent_ind_;
    assert( starInd >= 0 );
    const PiSourceDesc * star = &( system->bodies_[ starInd ] );

    //assert(star);

    /* while not found star yet.. */
    while ( star->super_type_ > SUPERTYPE_STAR)
    {
        planet = star;
        starInd = star->parent_ind_;
        star = &( system->bodies_[starInd] );
    }

    orbMin_ = planet->orb_min_;
    orbMax_ = planet->orb_max_;
    return star;
}

int PiSystemGenerator::calcSurfaceTemp( PiSystem * system, const PiSourceDesc & primary, fixed distToPrimary, fixed albedo, fixed greenhouse )
{
    // accumulator seeded with current primary
    fixed energy_per_meter2 = calcEnergyPerUnitAreaAtDist( primary.radius_, primary.average_temp_, distToPrimary );
    fixed dist;
    // find the other stars which aren't our parent star
    /*for (auto s : system->stars_ )
    {
        if (s != primary) {
            //get branches from body and star to system root
            Vector<const PiSourceDesc *> first_to_root;
            Vector<const PiSourceDesc *> second_to_root;
            getPathToRoot( primary, first_to_root );
            getPathToRoot( &(*s), second_to_root );
            Vector<const PiSourceDesc *>::Iterator fit = first_to_root.Begin();
            Vector<const PiSourceDesc *>::Iterator sit = second_to_root.Begin();
            while (sit != second_to_root.rend() && fit != first_to_root.rend() && (*sit) == (*fit)) //keep tracing both branches from system's root
            { //until they diverge
                ++sit;
                ++fit;
            }
            if (sit == second_to_root.rend()) --sit;
            if (fit == first_to_root.rend()) --fit; //oops! one of the branches ends at lca, backtrack

            if ((*fit)->IsCoOrbitalWith(*sit)) //planet is around one part of coorbiting pair, star is another.
            {
                dist = ((*fit)->GetOrbMaxAsFixed() + (*fit)->GetOrbMinAsFixed()) >> 1; //binaries don't have fully initialized smaxes
            }
			else if ( (*sit)->IsCoOrbital() ) //star is part of binary around which planet is (possibly indirectly) orbiting
            {
                bool inverted_ancestry = false;
                for (const PiSourceDesc * body = (*sit); body; body = body->parent_ )
                    if (body == (*fit))
					{
                        inverted_ancestry = true; //ugly hack due to function being static taking planet's primary rather than being called from actual planet
                        break;
                    }
                if (inverted_ancestry) //primary is star's ancestor! Don't try to take its orbit (could probably be a gravpoint check at this point, but paranoia)
                {
                    dist = distToPrimary;
                }
				else
				{
                    dist = ((*fit)->orb_max_ + (*fit)->orb_min_) >> 1; //simplified to planet orbiting stationary star
                }
            }
			else if ( (*fit)->IsCoOrbital() ) //planet is around one part of coorbiting pair, star isn't coorbiting with it
            {
                dist = ((*sit)->orb_max_ + (*sit)->orb_min_ ) >> 1; //simplified to star orbiting stationary planet
            }
			else //neither is part of any binaries - hooray!
            {
                dist = ( ( (*sit)->semimajor_axis_ - (*fit)->semimajor_axis_).Abs() //avg of conjunction and opposition dist
                           + ((*sit)->semimajor_axis_ + (*fit)->semimajor_axis_)) >>
                    1;
            }
        }
        energy_per_meter2 += calcEnergyPerUnitAreaAtDist( s->radius_, s->average_temp_, dist );
    }*/
    const fixed surface_temp_pow4 = energy_per_meter2 * ( 1 - albedo ) / ( 1 - greenhouse );
    return ( 279 * int( isqrt( isqrt( ( surface_temp_pow4.v) ) ) ) ) >> ( fixed::FRAC / 4 ); //multiplied by 279 to convert from Earth's temps to Kelvin
}

void PiSystemGenerator::pickAtmosphere( PiSourceDesc & sbody )
{
	/* Alpha value isn't real alpha. in the shader fog depth is determined
	* by density*alpha, so that we can have very dense atmospheres
	* without having them a big stinking solid color obscuring everything

	These are our atmosphere colours, for terrestrial planets we use m_atmosOxidizing
	for some variation to atmosphere colours
	*/
	switch ( sbody.type_ )
	{
	case TYPE_PLANET_GAS_GIANT:

		sbody.atmos_color_ = Color(255, 255, 255, 3);
		sbody.atmos_density_ = 14.0;
		break;
	case TYPE_PLANET_ASTEROID:
		sbody.atmos_color_ = Color::TRANSPARENT_BLACK; //Color::BLANK;
		sbody.atmos_density_ = 0.0;
		break;
	default:
	case TYPE_PLANET_TERRESTRIAL:
		double r = 0, g = 0, b = 0;
		double atmo = sbody.atm_oxidizing_.ToDouble();
		if ( sbody.gas_ > 0.001)
		{
			if (atmo > 0.95)
			{
				// o2
				r = 1.0f + ((0.95f - atmo) * 15.0f);
				g = 0.95f + ((0.95f - atmo) * 10.0f);
				b = atmo * atmo * atmo * atmo * atmo;
			}
			else if (atmo > 0.7)
			{
				// co2
				r = atmo + 0.05f;
				g = 1.0f + (0.7f - atmo);
				b = 0.8f;
			}
			else if (atmo > 0.65)
			{
				// co
				r = 1.0f + (0.65f - atmo);
				g = 0.8f;
				b = atmo + 0.25f;
			}
			else if (atmo > 0.55)
			{
				// ch4
				r = 1.0f + ((0.55f - atmo) * 5.0);
				g = 0.35f - ((0.55f - atmo) * 5.0);
				b = 0.4f;
			}
			else if (atmo > 0.3)
			{
				// h
				r = 1.0f;
				g = 1.0f;
				b = 1.0f;
			}
			else if (atmo > 0.2)
			{
				// he
				r = 1.0f;
				g = 1.0f;
				b = 1.0f;
			}
			else if (atmo > 0.15)
			{
				// ar
				r = 0.5f - ((0.15f - atmo) * 5.0);
				g = 0.0f;
				b = 0.5f + ((0.15f - atmo) * 5.0);
			}
			else if (atmo > 0.1)
			{
				// s
				r = 0.8f - ((0.1f - atmo) * 4.0);
				g = 1.0f;
				b = 0.5f - ((0.1f - atmo) * 10.0);
			}
			else
			{
				// n
				r = 1.0f;
				g = 1.0f;
				b = 1.0f;
			}
			sbody.atmos_color_ = Color(r, g, b, 1.0); //Color(r * 255, g * 255, b * 255, 255);
		}
		else
		{
			sbody.atmos_color_ = Color::TRANSPARENT_BLACK; //Color::BLANK;
		}
		sbody.atmos_density_ = sbody.gas_;
		//Output("| Atmosphere :\n|      red   : [%f] \n|      green : [%f] \n|      blue  : [%f] \n", r, g, b);
		//Output("-------------------------------\n");
		break;
		/*default:
		sbody.m_atmosColor = Color(0.6f, 0.6f, 0.6f, 1.0f);
		sbody.m_atmosDensity = m_body->m_volatileGas.ToDouble();
		break;*/
	}
}


static fixed mass_from_disk_area(fixed a, fixed b, fixed max)
{
    // so, density of the disk with distance from star goes like so: 1 - x/discMax
    //
    // ---
    //    ---
    //       --- <- zero at discMax
    //
    // Which turned into a disc becomes 2*pi*x - (2*pi*x*x)/discMax
    // Integral of which is: pi*x*x - (2/(3*discMax))*pi*x*x*x
    //
    // Because get_disc_density divides total_mass by
    // mass_from_disk_area(0, discMax, discMax) to find density, the
    // constant factors (pi) in this equation drop out.
    //
    b = (b > max ? max : b);
    assert(b >= a);
    assert(a <= max);
    assert(b <= max);
    assert(a >= 0);
    fixed one_over_3max = fixed(2, 1) / (3 * max);
    return (b * b - one_over_3max * b * b * b) -
        (a * a - one_over_3max * a * a * a);
}

static fixed get_disc_density( const PiSourceDesc & primary, fixed discMin, fixed discMax, fixed percentOfPrimaryMass )
{
    discMax = std::max( discMax, discMin );
    fixed total = mass_from_disk_area(discMin, discMax, discMax);
    return primary.mass_ * percentOfPrimaryMass / total;
}

static inline bool test_overlap( const fixed & x1, const fixed & x2, const fixed & y1, const fixed & y2 )
{
    return (x1 >= y1 && x1 <= y2) ||
        (x2 >= y1 && x2 <= y2) ||
        (y1 >= x1 && y1 <= x2) ||
        (y2 >= x1 && y2 <= x2);
}

/*
 * Instead we use these butt-ugly overflow-prone spat of ejaculate:
 */
/*
 * star_radius in sol radii
 * star_temp in kelvin,
 * object_dist in AU
 * return energy per unit area in solar constants (1362 W/m^2 )
 */
static fixed calcEnergyPerUnitAreaAtDist( fixed star_radius, int star_temp, fixed object_dist )
{
    fixed temp = star_temp * fixed(1, 5778); //normalize to Sun's temperature
    const fixed total_solar_emission =
        temp * temp * temp * temp * star_radius * star_radius;

    return total_solar_emission / (object_dist * object_dist); //return value in solar consts (overflow prevention)
}




}






