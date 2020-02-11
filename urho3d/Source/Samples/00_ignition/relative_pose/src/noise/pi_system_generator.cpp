
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
        planet->eccentricity_ = ecc;
        planet->axial_tilt_ = fixed(100, 157) * rand.NFixed(2);
        planet->semimajor_axis_ = semiMajorAxis;
        planet->type_ = TYPE_PLANET_TERRESTRIAL;
        planet->seed_ = rand.Int32();
        planet->parent_ = primary;
        planet->GM_ = mass;
        planet->rotation_period_ = fixed(rand.Int32(1, 200), 24);

        const double e = ecc.ToDouble();

        //if ( primary.type_ == TYPE_GRAVPOINT )
        //    planet->m_orbit.SetShapeAroundBarycentre( semiMajorAxis.ToDouble() * AU, primary.GM_, planet->GM_, e );
        //else
        //    planet->m_orbit.SetShapeAroundPrimary(semiMajorAxis.ToDouble() * AU, primary->GetMass(), e);

        //double r1 = rand.Double(2 * M_PI); // function parameter evaluation order is implementation-dependent
        //double r2 = rand.NDouble(5); // can't put two rands in the same expression
        //planet->m_orbit.SetPlane(matrix3x3d::RotateY(r1) * matrix3x3d::RotateX(-0.5 * M_PI + r2 * M_PI / 2.0));
        //planet->m_orbit.SetPhase(rand.Double(2 * M_PI));
        planet->X_ = rand.Double( 2 * M_PI );
        planet->Y_ = rand.NDouble( 5 );


        planet->inclination_ = FIXED_PI;
        planet->inclination_ *= r2 / 2.0;
        planet->orb_min_ = periapsis;
        planet->orb_max_ = apoapsis;
        primary.children_.Push( planet );

        //if (hasRings &&
        //    parentSuperType == SystemBody::SUPERTYPE_ROCKY_PLANET &&
        //    test_overlap(ring.minRadius, ring.maxRadius, periapsis, apoapsis)) {
        //    //Output("Overlap, eliminating rings from parent SystemBody\n");
        //    //Overlap, eliminating rings from parent SystemBody
        //    primary->m_rings.minRadius = fixed();
        //    primary->m_rings.maxRadius = fixed();
        //    primary->m_rings.baseColor = Color(255, 255, 255, 255);
        //}

        /* minimum separation between planets of 1.35 */
        pos = apoapsis * fixed( 135, 100 );
    }

    int idx = 0;
    bool make_moons = parentSuperType <= SUPERTYPE_STAR;

    for (Vector<PiSourceDesc *>::Iterator i = primary.children_.Begin(); i != primary.children_.End(); ++i)
    {
        // planets around a binary pair [gravpoint] -- ignore the stars...
        if ((*i)->super_type_ == SUPERTYPE_STAR)
            continue;
        // Turn them into something!!!!!!!
        char buf[12];
        if ( parentSuperType <= SUPERTYPE_STAR )
        {
            // planet naming scheme
            snprintf(buf, sizeof(buf), " %c", 'a' + idx);
        } else {
            // moon naming scheme
            snprintf(buf, sizeof(buf), " %d", 1 + idx);
        }
        //(*i)->m_name = primary->GetName() + buf;
        pickPlanetType( *i, rand );
        if ( make_moons )
            makePlanetsAround( system, *(*i), rand );
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













void PiSystemGenerator::pickPlanetType( PiSourceDesc * sbody, PiRandom & rand )
{
    fixed albedo;
    fixed greenhouse;

    fixed minDistToStar, maxDistToStar, averageDistToStar;
    const PiSourceDesc * star = FindStarAndTrueOrbitalRange( sbody, minDistToStar, maxDistToStar );
    averageDistToStar = (minDistToStar + maxDistToStar) >> 1;

    /* first calculate blackbody temp (no greenhouse effect, zero albedo) */
    int bbody_temp = CalcSurfaceTemp(star, averageDistToStar, albedo, greenhouse);

    sbody->m_averageTemp = bbody_temp;

    static const fixed ONEEUMASS = fixed::FromDouble(1);
    static const fixed TWOHUNDREDEUMASSES = fixed::FromDouble(200.0);
    // We get some more fractional bits for small bodies otherwise we can easily end up with 0 radius which breaks stuff elsewhere
    //
    // AndyC - Updated to use the empirically gathered data from this site:
    // http://phl.upr.edu/library/notes/standardmass-radiusrelationforexoplanets
    // but we still limit at the lowest end
    if (sbody->GetMassAsFixed() <= fixed(1, 1)) {
        sbody->m_radius = fixed(fixedf<48>::CubeRootOf(fixedf<48>(sbody->GetMassAsFixed())));
    } else if (sbody->GetMassAsFixed() < ONEEUMASS) {
        // smaller than 1 Earth mass is almost certainly a rocky body
        sbody->m_radius = fixed::FromDouble(pow(sbody->GetMassAsFixed().ToDouble(), 0.3));
    } else if (sbody->GetMassAsFixed() < TWOHUNDREDEUMASSES) {
        // from 1 EU to 200 they transition from Earth-like rocky bodies, through Ocean worlds and on to Gas Giants
        sbody->m_radius = fixed::FromDouble(pow(sbody->GetMassAsFixed().ToDouble(), 0.5));
    } else {
        // Anything bigger than 200 EU masses is a Gas Giant or bigger but the density changes to decrease from here on up...
        sbody->m_radius = fixed::FromDouble(22.6 * (1.0 / pow(sbody->GetMassAsFixed().ToDouble(), double(0.0886))));
    }
    // enforce minimum size of 10km
    sbody->m_radius = std::max(sbody->GetRadiusAsFixed(), fixed(1, 630));

    if (sbody->GetParent()->GetType() <= SystemBody::TYPE_STAR_MAX) {
        // get it from the table now rather than setting it on stars/gravpoints as
        // currently nothing else needs them to have metallicity
        sbody->m_metallicity = starMetallicities[sbody->GetParent()->GetType()] * rand.Fixed();
    } else {
        // this assumes the parent's parent is a star/gravpoint, which is currently always true
        sbody->m_metallicity = starMetallicities[sbody->GetParent()->GetParent()->GetType()] * rand.Fixed();
    }

    // harder to be volcanic when you are tiny (you cool down)
    sbody->m_volcanicity = std::min(fixed(1, 1), sbody->GetMassAsFixed()) * rand.Fixed();
    sbody->m_atmosOxidizing = rand.Fixed();
    sbody->m_life = fixed();
    sbody->m_volatileGas = fixed();
    sbody->m_volatileLiquid = fixed();
    sbody->m_volatileIces = fixed();

    // pick body type
    if (sbody->GetMassAsFixed() > 317 * 13) {
        // more than 13 jupiter masses can fuse deuterium - is a brown dwarf
        sbody->m_type = SystemBody::TYPE_BROWN_DWARF;
        sbody->m_averageTemp = sbody->GetAverageTemp() + rand.Int32(starTypeInfo[sbody->GetType()].tempMin, starTypeInfo[sbody->GetType()].tempMax);
        // prevent mass exceeding 65 jupiter masses or so, when it becomes a star
        // XXX since TYPE_BROWN_DWARF is supertype star, mass is now in
        // solar masses. what a fucking mess
        sbody->m_mass = std::min(sbody->GetMassAsFixed(), fixed(317 * 65, 1)) / SUN_MASS_TO_EARTH_MASS;
        //Radius is too high as it now uses the planetary calculations to work out radius (Cube root of mass)
        // So tell it to use the star data instead:
        sbody->m_radius = fixed(rand.Int32(starTypeInfo[sbody->GetType()].radius[0], starTypeInfo[sbody->GetType()].radius[1]), 100);
    } else if (sbody->GetMassAsFixed() > 6) {
        sbody->m_type = SystemBody::TYPE_PLANET_GAS_GIANT;
    } else if (sbody->GetMassAsFixed() > fixed(1, 15000)) {
        sbody->m_type = SystemBody::TYPE_PLANET_TERRESTRIAL;

        fixed amount_volatiles = fixed(2, 1) * rand.Fixed();
        if (rand.Int32(3)) amount_volatiles *= sbody->GetMassAsFixed();
        // total atmosphere loss
        if (rand.Fixed() > sbody->GetMassAsFixed()) amount_volatiles = fixed();

        //Output("Amount volatiles: %f\n", amount_volatiles.ToFloat());
        // fudge how much of the volatiles are in which state
        greenhouse = fixed();
        albedo = fixed();
        // CO2 sublimation
        if (sbody->GetAverageTemp() > 195)
            greenhouse += amount_volatiles * fixed(1, 3);
        else
            albedo += fixed(2, 6);
        // H2O liquid
        if (sbody->GetAverageTemp() > 273)
            greenhouse += amount_volatiles * fixed(1, 5);
        else
            albedo += fixed(3, 6);
        // H2O boils
        if (sbody->GetAverageTemp() > 373) greenhouse += amount_volatiles * fixed(1, 3);

        if (greenhouse > fixed(7, 10)) { // never reach 1, but 1/(1-greenhouse) still grows
            greenhouse *= greenhouse;
            greenhouse *= greenhouse;
            greenhouse = greenhouse / (greenhouse + fixed(32, 311));
        }

        sbody->m_averageTemp = CalcSurfaceTemp(star, averageDistToStar, albedo, greenhouse);

        const fixed proportion_gas = sbody->GetAverageTemp() / (fixed(100, 1) + sbody->GetAverageTemp());
        sbody->m_volatileGas = proportion_gas * amount_volatiles;

        const fixed proportion_liquid = (fixed(1, 1) - proportion_gas) * (sbody->GetAverageTemp() / (fixed(50, 1) + sbody->GetAverageTemp()));
        sbody->m_volatileLiquid = proportion_liquid * amount_volatiles;

        const fixed proportion_ices = fixed(1, 1) - (proportion_gas + proportion_liquid);
        sbody->m_volatileIces = proportion_ices * amount_volatiles;

        //Output("temp %dK, gas:liquid:ices %f:%f:%f\n", averageTemp, proportion_gas.ToFloat(),
        //		proportion_liquid.ToFloat(), proportion_ices.ToFloat());

        if ((sbody->GetVolatileLiquidAsFixed() > fixed()) &&
            (sbody->GetAverageTemp() > CELSIUS - 60) &&
            (sbody->GetAverageTemp() < CELSIUS + 200)) {
            // try for life
            int minTemp = CalcSurfaceTemp(star, maxDistToStar, albedo, greenhouse);
            int maxTemp = CalcSurfaceTemp(star, minDistToStar, albedo, greenhouse);

            if ((minTemp > CELSIUS - 10) && (minTemp < CELSIUS + 90) && //removed explicit checks for star type (also BD and WD seem to have slight chance of having life around them)
                (maxTemp > CELSIUS - 10) && (maxTemp < CELSIUS + 90)) //TODO: ceiling based on actual boiling point on the planet, not in 1atm
            {
                fixed maxMass, lifeMult, allowedMass(1, 2);
                allowedMass += 2;
                for (auto s : sbody->GetStarSystem()->GetStars()) { //find the most massive star, mass is tied to lifespan
                    maxMass = maxMass < s->GetMassAsFixed() ? s->GetMassAsFixed() : maxMass; //this automagically eliminates O, B and so on from consideration
                } //handy calculator: http://www.asc-csa.gc.ca/eng/educators/resources/astronomy/module2/calculator.asp
                if (maxMass < allowedMass) { //system could have existed long enough for life to form (based on Sol)
                    lifeMult = allowedMass - maxMass;
                }
                sbody->m_life = lifeMult * rand.Fixed();
            }
        }
    } else {
        sbody->m_type = SystemBody::TYPE_PLANET_ASTEROID;
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
    if (sbody->GetParent()->GetType() <= SystemBody::TYPE_STAR_MAX) {
        invTidalLockTime /= (sbody->GetSemiMajorAxisAsFixed() * sbody->GetSemiMajorAxisAsFixed());
        invTidalLockTime *= sbody->GetMassAsFixed();
        invTidalLockTime /= (sbody->GetSemiMajorAxisAsFixed() * sbody->GetSemiMajorAxisAsFixed());
        invTidalLockTime *= sbody->GetParent()->GetMassAsFixed() * sbody->GetParent()->GetMassAsFixed();
        invTidalLockTime /= sbody->GetRadiusAsFixed();
        invTidalLockTime /= (sbody->GetSemiMajorAxisAsFixed() * sbody->GetSemiMajorAxisAsFixed()) * MOON_TIDAL_LOCK;
    } else {
        invTidalLockTime /= (sbody->GetSemiMajorAxisAsFixed() * sbody->GetSemiMajorAxisAsFixed()) * SUN_MASS_TO_EARTH_MASS;
        invTidalLockTime *= sbody->GetMassAsFixed();
        invTidalLockTime /= (sbody->GetSemiMajorAxisAsFixed() * sbody->GetSemiMajorAxisAsFixed()) * SUN_MASS_TO_EARTH_MASS;
        invTidalLockTime *= sbody->GetParent()->GetMassAsFixed() * sbody->GetParent()->GetMassAsFixed();
        invTidalLockTime /= sbody->GetRadiusAsFixed();
        invTidalLockTime /= (sbody->GetSemiMajorAxisAsFixed() * sbody->GetSemiMajorAxisAsFixed()) * MOON_TIDAL_LOCK;
    }
    //Output("tidal lock of %s: %.5f, a %.5f R %.4f mp %.3f ms %.3f\n", name.c_str(),
    //		invTidalLockTime.ToFloat(), semiMajorAxis.ToFloat(), radius.ToFloat(), parent->mass.ToFloat(), mass.ToFloat());

    if (invTidalLockTime > 10) { // 10x faster than Moon, no chance not to be tidal-locked
        sbody->m_rotationPeriod = fixed(int(round(sbody->GetOrbit().Period())), 3600 * 24);
        sbody->m_axialTilt = sbody->GetInclinationAsFixed();
    } else if (invTidalLockTime > fixed(1, 100)) { // rotation speed changed in favour of tidal lock
        // XXX: there should be some chance the satellite was captured only recenly and ignore this
        //		I'm ommiting that now, I do not want to change the Universe by additional rand call.

        fixed lambda = invTidalLockTime / (fixed(1, 20) + invTidalLockTime);
        sbody->m_rotationPeriod = (1 - lambda) * sbody->GetRotationPeriodAsFixed() + lambda * sbody->GetOrbit().Period() / 3600 / 24;
        sbody->m_axialTilt = (1 - lambda) * sbody->GetAxialTiltAsFixed() + lambda * sbody->GetInclinationAsFixed();
    } // else .. nothing happens to the satellite

    PickAtmosphere(sbody);
    PickRings(sbody);
}

static fixed mass_from_disk_area(fixed a, fixed b, fixed max)
{
    PROFILE_SCOPED()
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

static fixed get_disc_density(SystemBody *primary, fixed discMin, fixed discMax, fixed percentOfPrimaryMass)
{
    PROFILE_SCOPED()
    discMax = std::max(discMax, discMin);
    fixed total = mass_from_disk_area(discMin, discMax, discMax);
    return primary->GetMassInEarths() * percentOfPrimaryMass / total;
}

static inline bool test_overlap(const fixed &x1, const fixed &x2, const fixed &y1, const fixed &y2)
{
    return (x1 >= y1 && x1 <= y2) ||
        (x2 >= y1 && x2 <= y2) ||
        (y1 >= x1 && y1 <= x2) ||
        (y2 >= x1 && y2 <= x2);
}



}






