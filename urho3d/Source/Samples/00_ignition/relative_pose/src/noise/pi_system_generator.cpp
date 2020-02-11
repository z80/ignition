
#include "pi_system_generator.h"
#include "pi_source.h"
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

const PiSystemGenerator::StarTypeInfo starTypeInfo[] = {
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

PiSystemGenerator::PiSystemGenerator()
{

}

PiSystemGenerator::~PiSystemGenerator()
{

}

void PiSystemGenerator::apply( PiRandom & rand )
{

}

void PiSystemGenerator::generateStars( PiRandom & rand )
{
    PiSystem s( 10, 10, 2, 3 );


    const int sx = s.sx;
    const int sy = s.sy;
    const int sz = s.sz;
    const Sint64 dist = (1 + sx * sx + sy * sy + sz * sz);
    const Sint64 freq = (1 + sx * sx + sy * sy);


    const int qty = rand.Int32(15);
    switch ( qty )
    {
    case 0:
        s.m_numStars = 4;
        break;
    case 1:
    case 2:
        s.m_numStars = 3;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        s.m_numStars = 2;
        break;
    default:
        s.m_numStars = 1;
        break;
    }



    if (freq > Square(10))
    {
        const Uint32 weight = rand.Int32(1000000);
        if (weight < 1) {
            s.m_starType[0] = TYPE_STAR_IM_BH; // These frequencies are made up
        } else if (weight < 3) {
            s.m_starType[0] = TYPE_STAR_S_BH;
        } else if (weight < 5) {
            s.m_starType[0] = TYPE_STAR_O_WF;
        } else if (weight < 8) {
            s.m_starType[0] = TYPE_STAR_B_WF;
        } else if (weight < 12) {
            s.m_starType[0] = TYPE_STAR_M_WF;
        } else if (weight < 15) {
            s.m_starType[0] = TYPE_STAR_K_HYPER_GIANT;
        } else if (weight < 18) {
            s.m_starType[0] = TYPE_STAR_G_HYPER_GIANT;
        } else if (weight < 23) {
            s.m_starType[0] = TYPE_STAR_O_HYPER_GIANT;
        } else if (weight < 28) {
            s.m_starType[0] = TYPE_STAR_A_HYPER_GIANT;
        } else if (weight < 33) {
            s.m_starType[0] = TYPE_STAR_F_HYPER_GIANT;
        } else if (weight < 41) {
            s.m_starType[0] = TYPE_STAR_B_HYPER_GIANT;
        } else if (weight < 48) {
            s.m_starType[0] = TYPE_STAR_M_HYPER_GIANT;
        } else if (weight < 58) {
            s.m_starType[0] = TYPE_STAR_K_SUPER_GIANT;
        } else if (weight < 68) {
            s.m_starType[0] = TYPE_STAR_G_SUPER_GIANT;
        } else if (weight < 78) {
            s.m_starType[0] = TYPE_STAR_O_SUPER_GIANT;
        } else if (weight < 88) {
            s.m_starType[0] = TYPE_STAR_A_SUPER_GIANT;
        } else if (weight < 98) {
            s.m_starType[0] = TYPE_STAR_F_SUPER_GIANT;
        } else if (weight < 108) {
            s.m_starType[0] = TYPE_STAR_B_SUPER_GIANT;
        } else if (weight < 158) {
            s.m_starType[0] = TYPE_STAR_M_SUPER_GIANT;
        } else if (weight < 208) {
            s.m_starType[0] = TYPE_STAR_K_GIANT;
        } else if (weight < 250) {
            s.m_starType[0] = TYPE_STAR_G_GIANT;
        } else if (weight < 300) {
            s.m_starType[0] = TYPE_STAR_O_GIANT;
        } else if (weight < 350) {
            s.m_starType[0] = TYPE_STAR_A_GIANT;
        } else if (weight < 400) {
            s.m_starType[0] = TYPE_STAR_F_GIANT;
        } else if (weight < 500) {
            s.m_starType[0] = TYPE_STAR_B_GIANT;
        } else if (weight < 700) {
            s.m_starType[0] = TYPE_STAR_M_GIANT;
        } else if (weight < 800) {
            s.m_starType[0] = TYPE_STAR_O; // should be 1 but that is boring
        } else if (weight < 2000) { // weight < 1300 / 20500
            s.m_starType[0] = TYPE_STAR_B;
        } else if (weight < 8000) { // weight < 7300
            s.m_starType[0] = TYPE_STAR_A;
        } else if (weight < 37300) { // weight < 37300
            s.m_starType[0] = TYPE_STAR_F;
        } else if (weight < 113300) { // weight < 113300
            s.m_starType[0] = TYPE_STAR_G;
        } else if (weight < 234300) { // weight < 234300
            s.m_starType[0] = TYPE_STAR_K;
        } else if (weight < 250000) { // weight < 250000
            s.m_starType[0] = TYPE_WHITE_DWARF;
        } else if (weight < 900000) { //weight < 900000
            s.m_starType[0] = TYPE_STAR_M;
        } else {
            s.m_starType[0] = TYPE_BROWN_DWARF;
        }
    } else {
        const Uint32 weight = rand.Int32(1000000);
        if (weight < 100) { // should be 1 but that is boring
            s.m_starType[0] = TYPE_STAR_O;
        } else if (weight < 1300) {
            s.m_starType[0] = TYPE_STAR_B;
        } else if (weight < 7300) {
            s.m_starType[0] = TYPE_STAR_A;
        } else if (weight < 37300) {
            s.m_starType[0] = TYPE_STAR_F;
        } else if (weight < 113300) {
            s.m_starType[0] = TYPE_STAR_G;
        } else if (weight < 234300) {
            s.m_starType[0] = TYPE_STAR_K;
        } else if (weight < 250000) {
            s.m_starType[0] = TYPE_WHITE_DWARF;
        } else if (weight < 900000) {
            s.m_starType[0] = TYPE_STAR_M;
        } else {
            s.m_starType[0] = TYPE_BROWN_DWARF;
        }
    }
    //Output("%d: %d%\n", sx, sy);

    if (s.m_numStars > 1) {
        s.m_starType[1] = BodyType(rand.Int32(TYPE_STAR_MIN, s.m_starType[0]));
        if (s.m_numStars > 2) {
            s.m_starType[2] = BodyType(rand.Int32(TYPE_STAR_MIN, s.m_starType[0]));
            s.m_starType[3] = BodyType(rand.Int32(TYPE_STAR_MIN, s.m_starType[2]));
        }
    }

    if ((s.m_starType[0] <= TYPE_STAR_A) && (rand.Int32(10) == 0)) {
        // make primary a giant. never more than one giant in a system
        if (freq > Square(10)) {
            const Uint32 weight = rand.Int32(1000);
            if (weight >= 999) {
                s.m_starType[0] = TYPE_STAR_B_HYPER_GIANT;
            } else if (weight >= 998) {
                s.m_starType[0] = TYPE_STAR_O_HYPER_GIANT;
            } else if (weight >= 997) {
                s.m_starType[0] = TYPE_STAR_K_HYPER_GIANT;
            } else if (weight >= 995) {
                s.m_starType[0] = TYPE_STAR_B_SUPER_GIANT;
            } else if (weight >= 993) {
                s.m_starType[0] = TYPE_STAR_O_SUPER_GIANT;
            } else if (weight >= 990) {
                s.m_starType[0] = TYPE_STAR_K_SUPER_GIANT;
            } else if (weight >= 985) {
                s.m_starType[0] = TYPE_STAR_B_GIANT;
            } else if (weight >= 980) {
                s.m_starType[0] = TYPE_STAR_O_GIANT;
            } else if (weight >= 975) {
                s.m_starType[0] = TYPE_STAR_K_GIANT;
            } else if (weight >= 950) {
                s.m_starType[0] = TYPE_STAR_M_HYPER_GIANT;
            } else if (weight >= 875) {
                s.m_starType[0] = TYPE_STAR_M_SUPER_GIANT;
            } else {
                s.m_starType[0] = TYPE_STAR_M_GIANT;
            }
        } else if (freq > Square(5))
            s.m_starType[0] = TYPE_STAR_M_GIANT;
        else
            s.m_starType[0] = TYPE_STAR_M;

        //Output("%d: %d%\n", sx, sy);
    }

    //s.m_name = GenName(galaxy, *sector, s, customCount + i, rng);
    //Output("%s: \n", s.m_name.c_str());

    //s.m_systems.push_back(s);
}

void PiSystemGenerator::makePlanetsAround( PiSystem & system, PiSourceDesc & primary, PiRandom & rand )
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
            discMin = primary.children_[0]->orb_min_ * SAFE_DIST_FROM_BINARY;
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
            discMax = 100 * rand.NFixed(2) * fixed::SqrtOf(primary.GM_);
        }
        // having limited discMin by bin-separation/fake roche, and
        // discMax by some relation to star mass, we can now compute
        // disc density
        discDensity = rand.Fixed() * get_disc_density( primary, discMin, discMax, fixed(2, 100) );

        if ( (parentSuperType == SUPERTYPE_STAR) && ( primary.parent_ ) )
        {
            // limit planets out to 10% distance to star's binary companion
            discMax = std::min( discMax, primary.orb_min_ * fixed(1, 10) );
        }

        /* in trinary and quaternary systems don't bump into other pair... */
        if ( system.m_numStars >= 3 )
        {
            discMax = std::min( discMax, fixed(5, 100) * system.root_body_->children_[0]->orb_min_ );
        }
    }
    else
    {
        fixed primary_rad = primary.radius_ * AU_EARTH_RADIUS;
        discMin = 4 * primary_rad;
        /* use hill radius to find max size of moon system. for stars botch it.
           And use planets orbit around its primary as a scaler to a moon's orbit*/
        discMax = std::min( discMax, fixed(1, 20) * CalcHillRadius(primary) * primary.orb_min_ * fixed(1, 10) );

        discDensity = rand.Fixed() * get_disc_density( primary, discMin, discMax, fixed(1, 500) );
    }

    //fixed discDensity = 20*rand.NFixed(4);

    //Output("Around %s: Range %f -> %f AU\n", primary->GetName().c_str(), discMin.ToDouble(), discMax.ToDouble());

    fixed initialJump = rand.NFixed(5);
    fixed pos = (fixed(1, 1) - initialJump) * discMin + (initialJump * discMax);
    const RingStyle & ring = primary->GetRings();
    const bool hasRings = primary->HasRings();

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
            //Output("WARNING: planetary mass has overflowed! (child of %s)\n", primary->GetName().c_str());
            mass = fixed(Sint64(0x7fFFffFFffFFffFFull));
        }
        assert(mass >= 0);

        PiSourceDesc * planet = new PiSourceDesc(); //system->NewBody();
        planet->m_eccentricity = ecc;
        planet->m_axialTilt = fixed(100, 157) * rand.NFixed(2);
        planet->m_semiMajorAxis = semiMajorAxis;
        planet->m_type = TYPE_PLANET_TERRESTRIAL;
        planet->m_seed = rand.Int32();
        planet->m_parent = primary;
        planet->m_mass = mass;
        planet->m_rotationPeriod = fixed(rand.Int32(1, 200), 24);

        const double e = ecc.ToDouble();

        if ( primary.type_ == TYPE_GRAVPOINT )
            planet->m_orbit.SetShapeAroundBarycentre( semiMajorAxis.ToDouble() * AU, primary.GM_, planet->GM_, e );
        else
            planet->m_orbit.SetShapeAroundPrimary(semiMajorAxis.ToDouble() * AU, primary->GetMass(), e);

        double r1 = rand.Double(2 * M_PI); // function parameter evaluation order is implementation-dependent
        double r2 = rand.NDouble(5); // can't put two rands in the same expression
        planet->m_orbit.SetPlane(matrix3x3d::RotateY(r1) * matrix3x3d::RotateX(-0.5 * M_PI + r2 * M_PI / 2.0));
        planet->m_orbit.SetPhase(rand.Double(2 * M_PI));

        planet->m_inclination = FIXED_PI;
        planet->m_inclination *= r2 / 2.0;
        planet->orb_min_ = periapsis;
        planet->orb_max_ = apoapsis;
        primary.children_.Push( planet );

        if (hasRings &&
            parentSuperType == SystemBody::SUPERTYPE_ROCKY_PLANET &&
            test_overlap(ring.minRadius, ring.maxRadius, periapsis, apoapsis)) {
            //Output("Overlap, eliminating rings from parent SystemBody\n");
            //Overlap, eliminating rings from parent SystemBody
            primary->m_rings.minRadius = fixed();
            primary->m_rings.maxRadius = fixed();
            primary->m_rings.baseColor = Color(255, 255, 255, 255);
        }

        /* minimum separation between planets of 1.35 */
        pos = apoapsis * fixed(135, 100);
    }

    int idx = 0;
    bool make_moons = parentSuperType <= SUPERTYPE_STAR;

    for (std::vector<SystemBody *>::iterator i = primary->m_children.begin(); i != primary->m_children.end(); ++i)
    {
        // planets around a binary pair [gravpoint] -- ignore the stars...
        if ((*i)->GetSuperType() == SUPERTYPE_STAR) continue;
        // Turn them into something!!!!!!!
        char buf[12];
        if (parentSuperType <= SUPERTYPE_STAR)
        {
            // planet naming scheme
            snprintf(buf, sizeof(buf), " %c", 'a' + idx);
        } else {
            // moon naming scheme
            snprintf(buf, sizeof(buf), " %d", 1 + idx);
        }
        (*i)->m_name = primary->GetName() + buf;
        PickPlanetType(*i, rand);
        if ( make_moons )
            MakePlanetsAround(system, *i, rand);
        idx++;
    }
}

void PiSystemGenerator::makeRandomStar( PiSourceDesc & sbody, PiRandom & rand )
{

}

void PiSystemGenerator::makeStarOfType( PiSourceDesc & sbody, BodyType type, PiRandom & rand )
{

}

void PiSystemGenerator::makeStarOfTypeLighterThan( PiSourceDesc & sbody, BodyType type, fixed maxMass, PiRandom & rand )
{

}



}






