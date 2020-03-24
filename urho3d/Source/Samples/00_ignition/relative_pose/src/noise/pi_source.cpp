
#define NOMINMAX

#include "pi_source.h"
#include "pi_consts.h"
#include <algorithm>

static  fixed Clamp( fixed v, Float vmin, Float vmax )
{
	fixed r;
	if ( v < vmin )
		r = vmin;
	else if ( v > vmax )
		r = vmax;
	else
		r = v;
	return r;
}

#define COUNTOF( arr ) (sizeof(arr)/sizeof(arr[0]))


namespace Ign
{

template <typename HeightFractal, typename ColorFractal>
static PiBodySource * InstanceGenerator( const PiSourceDesc & desc )
{
    return new TerrainGenerator<HeightFractal, ColorFractal>( desc );
}

typedef PiBodySource * ( * GeneratorInstancer )( const PiSourceDesc & desc );


PiSourceDesc::PiSourceDesc()
{
    seed_ = 0;

    super_type_ = SUPERTYPE_ROCKY_PLANET;
    type_       = TYPE_PLANET_TERRESTRIAL;

    mass_ = 1.0;
	radius_ = 30.0;
	aspectRatio_ = 1.0;

    life_ = 0.1;
    gas_ = 0.05;
    average_temp_ = 240;
    liquid_ = 0.5;
    volcanic_ = 0.2;
    ice_ = 0.2;
    metal_ = 0.1;

    //parent_ = nullptr;
	parent_ind_ = -1;

    orb_max_ = 0;
    orb_min_ = 0;
    eccentricity_ = 0;
    axial_tilt_ = 0;
    semimajor_axis_ = 0;
    rotation_period_ = 60*60*24;
    inclination_ = 0;
    start_eccentric_anomaly_ = 0;
    Y_ = 0;
    X_ = 0;
}

Float PiSourceDesc::mass() const
{
    Float m = mass_.ToDouble();
    if ( super_type_ == SUPERTYPE_STAR )
        m *= SOL_MASS;
    else if ( ( super_type_ == SUPERTYPE_ROCKY_PLANET ) || 
              ( super_type_ == SUPERTYPE_GAS_GIANT ) )
        m *= EARTH_MASS;
    return m;
}

Float PiSourceDesc::radius() const
{
    Float r = radius_.ToDouble();
    if ( super_type_ == SUPERTYPE_STAR )
        r *= SOL_RADIUS;
    else if ( ( super_type_ == SUPERTYPE_ROCKY_PLANET ) || 
        ( super_type_ == SUPERTYPE_GAS_GIANT ) )
        r *= EARTH_RADIUS;
    return r;
}

PiBodySource::PiBodySource( const PiSourceDesc & body )
    : HeightSource(), 
	m_seed(body.seed_),
	m_rand(body.seed_),
	//m_heightScaling(0),
	m_minh(0)

{
	m_heightScaling = 1.0;

	m_minBody = body;



	m_sealevel = Clamp(body.liquid_.ToDouble(), 0.0, 1.0);
	m_icyness = Clamp(body.ice_.ToDouble(), 0.0, 1.0);
	m_volcanic = Clamp(body.volcanic_.ToDouble(), 0.0, 1.0); // height scales with volcanicity as well
	m_surfaceEffects = 0;

    // Radius in meters.
	const Float rad = m_minBody.radius();
    // mass in kilograms.
    const Float mass = body.mass();
    // calculate max height
    const Float empiricHeight = (9000.0 * rad * rad * (m_volcanic + 0.5)) / ( mass * 6.64e-12 );
	// max mountain height for earth-like planet (same mass, radius)
	m_maxHeightInMeters = std::max(100.0, empiricHeight );
	m_maxHeightInMeters = std::min(rad, m_maxHeightInMeters); // small asteroid case

	// and then in sphere normalized jizz
	m_maxHeight = m_maxHeightInMeters / rad;
	//Output("%s: max terrain height: %fm [%f]\n", m_minBody.name.c_str(), m_maxHeightInMeters, m_maxHeight);
	m_invMaxHeight = 1.0 / m_maxHeight;
	m_planetRadius = rad;
	m_invPlanetRadius = 1.0 / rad;
	m_planetEarthRadii = rad / EARTH_RADIUS;

	// Pick some colors, mainly reds and greens
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_rockColor)); i++)
	{
		double r, g, b;
		r = m_rand.Double(0.3, 1.0);
		g = m_rand.Double(0.3, r);
		b = m_rand.Double(0.3, g);
		r = std::max(b, r * body.metal_.ToDouble());
		g = std::max(b, g * body.metal_.ToDouble());
		m_rockColor[i] = Vector3d(r, g, b);
	}

	// Pick some darker colours mainly reds and greens
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_darkrockColor)); i++)
	{
		Float r, g, b;
		r = m_rand.Double(0.05, 0.3);
		g = m_rand.Double(0.05, r);
		b = m_rand.Double(0.05, g);
		r = std::max(b, r * body.metal_.ToDouble());
		g = std::max(b, g * body.metal_.ToDouble());
		m_darkrockColor[i] = Vector3d(r, g, b);
	}

	// grey colours, in case you simply must have a grey colour on a world with high metallicity
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_greyrockColor)); i++)
	{
		Float g;
		g = m_rand.Double(0.3, 0.9);
		m_greyrockColor[i] = Vector3d(g, g, g);
	}

	// Pick some plant colours, mainly greens
	// TODO take star class into account
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_plantColor)); i++)
	{
		Float r, g, b;
		g = m_rand.Double(0.3, 1.0);
		r = m_rand.Double(0.3, g);
		b = m_rand.Double(0.2, r);
		g = std::max(r, g * body.life_.ToDouble());
		b *= (1.0 - body.life_.ToDouble());
		m_plantColor[i] = Vector3d(r, g, b);
	}

	// Pick some darker plant colours mainly greens
	// TODO take star class into account
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_darkplantColor)); i++)
	{
		Float r, g, b;
		g = m_rand.Double(0.05, 0.3);
		r = m_rand.Double(0.00, g);
		b = m_rand.Double(0.00, r);
		g = std::max(r, g * body.life_.ToDouble());
		b *= (1.0 - body.life_.ToDouble());
		m_darkplantColor[i] = Vector3d(r, g, b);
	}

	// Pick some sand colours, mainly yellow
	// TODO let some planetary value scale this colour
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_sandColor)); i++)
	{
		Float r, g, b;
		r = m_rand.Double(0.6, 1.0);
		g = m_rand.Double(0.6, r);
		//b = m_rand.Double(0.0, g/2.0);
		b = 0;
		m_sandColor[i] = Vector3d(r, g, b);
	}

	// Pick some darker sand colours mainly yellow
	// TODO let some planetary value scale this colour
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_darksandColor)); i++)
	{
		Float r, g, b;
		r = m_rand.Double(0.05, 0.6);
		g = m_rand.Double(0.00, r);
		//b = m_rand.Double(0.00, g/2.0);
		b = 0;
		m_darksandColor[i] = Vector3d(r, g, b);
	}

	// Pick some dirt colours, mainly red/brown
	// TODO let some planetary value scale this colour
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_dirtColor)); i++)
	{
		Float r, g, b;
		r = m_rand.Double(0.3, 0.7);
		g = m_rand.Double(r - 0.1, 0.75);
		b = m_rand.Double(0.0, r / 2.0);
		m_dirtColor[i] = Vector3d(r, g, b);
	}

	// Pick some darker dirt colours mainly red/brown
	// TODO let some planetary value scale this colour
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_darkdirtColor)); i++)
	{
		double r, g, b;
		r = m_rand.Double(0.05, 0.3);
		g = m_rand.Double(r - 0.05, 0.35);
		b = m_rand.Double(0.0, r / 2.0);
		m_darkdirtColor[i] = Vector3d(r, g, b);
	}

	// These are used for gas giant colours, they are more m_random and *should* really use volatileGasses - TODO
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_gglightColor)); i++)
	{
		double r, g, b;
		r = m_rand.Double(0.0, 0.5);
		g = m_rand.Double(0.0, 0.5);
		b = m_rand.Double(0.0, 0.5);
		m_gglightColor[i] = Vector3d(r, g, b);
	}
	//darker gas giant colours, more reds and greens
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_ggdarkColor)); i++)
	{
		double r, g, b;
		r = m_rand.Double(0.0, 0.3);
		g = m_rand.Double(0.0, r);
		b = m_rand.Double(0.0, std::min(r, g));
		m_ggdarkColor[i] = Vector3d(r, g, b);
	}
}

PiBodySource::~PiBodySource()
{
}

Float PiBodySource::height(const Vector3d& at) const
{
    const Float h = GetHeight(at);
    return h;
}

Color PiBodySource::color(const Vector3d& at) const
{
    const Float h = GetHeight( at );
    const Vector3d c = GetColor( at, h, at );
    const Color col(c.x_, c.y_, c.z_, 1.0);
    return col;
}

void PiBodySource::SetFracDef( const unsigned int index, const Float featureHeightMeters, const Float featureWidthMeters, const Float smallestOctaveMeters )
{
	assert(index >= 0 && index < MAX_FRACDEFS);
	// feature
	m_fracdef[index].amplitude = featureHeightMeters / (m_maxHeight * m_planetRadius);
	m_fracdef[index].frequency = m_planetRadius / featureWidthMeters;
	m_fracdef[index].octaves = std::max(1, int(ceil(log(featureWidthMeters / smallestOctaveMeters) / log(2.0))));
	m_fracdef[index].lacunarity = 2.0;
	//Output("%d octaves\n", m_fracdef[index].octaves); //print
}

Float PiBodySource::BiCubicInterpolation( const Vector3d & p ) const
{
	const Float h = height( p );
	return h;
}









































PiBodySource * PiBodySource::InstanceTerrain( const PiSourceDesc & body )
{
	PiRandom rand(body.seed_);

	GeneratorInstancer gi = nullptr;

	switch ( body.type_ )
	{


    case TYPE_PLANET_GAS_GIANT: {
		const GeneratorInstancer choices[] = {
			InstanceGenerator<TerrainHeightFlat, TerrainColorGGJupiter>,
			InstanceGenerator<TerrainHeightFlat, TerrainColorGGSaturn>,
			InstanceGenerator<TerrainHeightFlat, TerrainColorGGSaturn2>,
			InstanceGenerator<TerrainHeightFlat, TerrainColorGGNeptune>,
			InstanceGenerator<TerrainHeightFlat, TerrainColorGGNeptune2>,
			InstanceGenerator<TerrainHeightFlat, TerrainColorGGUranus>,
			InstanceGenerator<TerrainHeightFlat, TerrainColorGGSaturn>
		};
		gi = choices[rand.Int32(COUNTOF(choices))];
		break;
	}

    case TYPE_PLANET_ASTEROID: {
		const GeneratorInstancer choices[] = {
			InstanceGenerator<TerrainHeightAsteroid, TerrainColorAsteroid>,
			InstanceGenerator<TerrainHeightAsteroid2, TerrainColorAsteroid>,
			InstanceGenerator<TerrainHeightAsteroid3, TerrainColorAsteroid>,
			InstanceGenerator<TerrainHeightAsteroid4, TerrainColorAsteroid>,
			InstanceGenerator<TerrainHeightAsteroid, TerrainColorRock>,
			InstanceGenerator<TerrainHeightAsteroid2, TerrainColorBandedRock>,
			InstanceGenerator<TerrainHeightAsteroid3, TerrainColorRock>,
			InstanceGenerator<TerrainHeightAsteroid4, TerrainColorBandedRock>
		};
        const uint32_t qty = COUNTOF(choices);
        const uint32_t ind = rand.Int32( qty );
		gi = choices[ind];
		break;
	}

    case TYPE_PLANET_TERRESTRIAL: {

		//Over-ride:
		//gi = InstanceGenerator<TerrainHeightAsteroid3,TerrainColorRock>;
		//break;
		// Earth-like world

		if ((body.life_ > fixed(7, 10)) && (body.gas_ > fixed(2, 10))) {
			// There would be no life on the surface without atmosphere

			if (body.average_temp_ > 240) {
				const GeneratorInstancer choices[] = {
					InstanceGenerator<TerrainHeightHillsRidged, TerrainColorEarthLike>,
					InstanceGenerator<TerrainHeightHillsRivers, TerrainColorEarthLike>,
					InstanceGenerator<TerrainHeightHillsDunes, TerrainColorEarthLike>,
					InstanceGenerator<TerrainHeightMountainsRidged, TerrainColorEarthLike>,
					InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorEarthLike>,
					InstanceGenerator<TerrainHeightMountainsRivers, TerrainColorEarthLike>,
					InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorEarthLike>,
					InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorEarthLike>
				};
				gi = choices[rand.Int32(COUNTOF(choices))];
				break;
			}

			const GeneratorInstancer choices[] = {
				InstanceGenerator<TerrainHeightHillsRidged, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightHillsRivers, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightHillsDunes, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightMountainsRidged, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightMountainsRivers, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightBarrenRock, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorDesert> //,
																				//InstanceGenerator<TerrainHeightBarrenRock3,TerrainColorTFGood>
			};
			gi = choices[rand.Int32(COUNTOF(choices))];
			break;
		}

		// Harsh, habitable world
		if ((body.gas_ > fixed(2, 10)) && (body.life_ > fixed(4, 10))) {

			if (body.average_temp_ > 240) {
				const GeneratorInstancer choices[] = {
					InstanceGenerator<TerrainHeightHillsRidged, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightHillsRivers, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightHillsDunes, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightHillsNormal, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightMountainsRidged, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightMountainsRivers, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightBarrenRock, TerrainColorTFGood>,
					InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorTFGood>
					//InstanceGenerator<TerrainHeightBarrenRock3,TerrainColorTFGood>
				};
				gi = choices[rand.Int32(COUNTOF(choices))];
				break;
			}

			const GeneratorInstancer choices[] = {
				InstanceGenerator<TerrainHeightHillsRidged, TerrainColorIce>,
				InstanceGenerator<TerrainHeightHillsRivers, TerrainColorIce>,
				InstanceGenerator<TerrainHeightHillsDunes, TerrainColorIce>,
				InstanceGenerator<TerrainHeightHillsNormal, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsRidged, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsRivers, TerrainColorIce>,
				InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock3, TerrainColorIce>
			};
			gi = choices[rand.Int32(COUNTOF(choices))];
			break;
		}

		// Marginally habitable world/ verging on mars like :)
		else if ((body.gas_ > fixed(1, 10)) && (body.life_ > fixed(1, 10))) {

			if (body.average_temp_ > 240) {
				const GeneratorInstancer choices[] = {
					InstanceGenerator<TerrainHeightHillsRidged, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightHillsRivers, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightHillsDunes, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightHillsNormal, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightMountainsRidged, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightMountainsRivers, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightBarrenRock, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorTFPoor>,
					InstanceGenerator<TerrainHeightBarrenRock3, TerrainColorTFPoor>
				};
				gi = choices[rand.Int32(COUNTOF(choices))];
				break;
			}

			const GeneratorInstancer choices[] = {
				InstanceGenerator<TerrainHeightHillsRidged, TerrainColorIce>,
				InstanceGenerator<TerrainHeightHillsRivers, TerrainColorIce>,
				InstanceGenerator<TerrainHeightHillsDunes, TerrainColorIce>,
				InstanceGenerator<TerrainHeightHillsNormal, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsRidged, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsRivers, TerrainColorIce>,
				InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock3, TerrainColorIce>
			};
			gi = choices[rand.Int32(COUNTOF(choices))];
			break;
		}

		// Desert-like world, Mars -like.
		if ((body.liquid_ < fixed(1, 10)) && (body.gas_ > fixed(1, 5))) {
			const GeneratorInstancer choices[] = {
				InstanceGenerator<TerrainHeightHillsDunes, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightWaterSolid, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightRuggedLava, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightBarrenRock, TerrainColorDesert>,
				InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorDesert>
			};
			gi = choices[rand.Int32(COUNTOF(choices))];
			break;
		}

		// Frozen world
		if ((body.ice_ > fixed(8, 10)) && (body.average_temp_ < 250)) {
			const GeneratorInstancer choices[] = {
				InstanceGenerator<TerrainHeightHillsDunes, TerrainColorIce>,
				InstanceGenerator<TerrainHeightHillsCraters, TerrainColorIce>,
				InstanceGenerator<TerrainHeightMountainsCraters, TerrainColorIce>,
				InstanceGenerator<TerrainHeightWaterSolid, TerrainColorIce>,
				InstanceGenerator<TerrainHeightWaterSolidCanyons, TerrainColorIce>,
				InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock3, TerrainColorIce>
			};
			gi = choices[rand.Int32(COUNTOF(choices))];
			break;
		}

		// Volcanic world
		if (body.volcanic_ > fixed(7, 10)) {

			if (body.life_ > fixed(5, 10)) // life on a volcanic world ;)
				gi = InstanceGenerator<TerrainHeightRuggedLava, TerrainColorTFGood>;
			else if (body.life_ > fixed(2, 10))
				gi = InstanceGenerator<TerrainHeightRuggedLava, TerrainColorTFPoor>;
			else
				gi = InstanceGenerator<TerrainHeightRuggedLava, TerrainColorVolcanic>;
			break;
		}

		//Below might not be needed.
		//Alien life world:
		if (body.life_ > fixed(1, 10)) {
			const GeneratorInstancer choices[] = {
				InstanceGenerator<TerrainHeightHillsDunes, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightHillsRidged, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightHillsRivers, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightMountainsRidged, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightMountainsVolcano, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightMountainsRiversVolcano, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightMountainsRivers, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightWaterSolid, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightRuggedLava, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorTFPoor>,
				InstanceGenerator<TerrainHeightBarrenRock, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorIce>,
				InstanceGenerator<TerrainHeightBarrenRock3, TerrainColorIce>
			};
			gi = choices[rand.Int32(COUNTOF(choices))];
			break;
        }

		if (body.gas_ > fixed(1, 10)) {
			const GeneratorInstancer choices[] = {
				InstanceGenerator<TerrainHeightHillsNormal, TerrainColorRock>,
				InstanceGenerator<TerrainHeightMountainsNormal, TerrainColorRock>,
				InstanceGenerator<TerrainHeightRuggedDesert, TerrainColorRock>,
				InstanceGenerator<TerrainHeightBarrenRock, TerrainColorRock>,
				InstanceGenerator<TerrainHeightBarrenRock2, TerrainColorRock>,
				InstanceGenerator<TerrainHeightBarrenRock3, TerrainColorRock>
			};
			gi = choices[rand.Int32(COUNTOF(choices))];
			break;
		}

		const GeneratorInstancer choices[] = {
			InstanceGenerator<TerrainHeightHillsCraters2, TerrainColorRock>,
			InstanceGenerator<TerrainHeightMountainsCraters2, TerrainColorRock>,
			InstanceGenerator<TerrainHeightBarrenRock3, TerrainColorRock>
		};
		gi = choices[rand.Int32(COUNTOF(choices))];
		break;
	}

	default:
		gi = InstanceGenerator<TerrainHeightFlat, TerrainColorWhite>;
		break;
	}

	return gi(body);
}

PiBodySource * PiBodySource::InstanceStar( const PiSourceDesc & body )
{
    PiRandom rand(body.seed_);

    GeneratorInstancer gi = nullptr;

    switch (body.type_)
    {

    case TYPE_BROWN_DWARF:
        gi = InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarBrownDwarf>;
        break;

    case TYPE_WHITE_DWARF:
        gi = InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarWhiteDwarf>;
        break;

    case TYPE_STAR_M:
    case TYPE_STAR_M_GIANT:
    case TYPE_STAR_M_SUPER_GIANT:
    case TYPE_STAR_M_HYPER_GIANT:
    {
        const GeneratorInstancer choices[] = {
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarM>,
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarM>,
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarK>,
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarG>
        };
        gi = choices[rand.Int32(COUNTOF(choices))];
        break;
    }

    case TYPE_STAR_K:
    case TYPE_STAR_K_GIANT:
    case TYPE_STAR_K_SUPER_GIANT:
    case TYPE_STAR_K_HYPER_GIANT:
    {
        const GeneratorInstancer choices[] = {
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarM>,
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarK>,
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarK>,
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarG>
        };
        gi = choices[rand.Int32(COUNTOF(choices))];
        break;
    }

    case TYPE_STAR_G:
    case TYPE_STAR_G_GIANT:
    case TYPE_STAR_G_SUPER_GIANT:
    case TYPE_STAR_G_HYPER_GIANT:
    {
        const GeneratorInstancer choices[] = {
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarWhiteDwarf>,
            InstanceGenerator<TerrainHeightEllipsoid, TerrainColorStarG>
        };
        gi = choices[rand.Int32(COUNTOF(choices))];
        break;
    }

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
        gi = InstanceGenerator<TerrainHeightEllipsoid, TerrainColorWhite>;
        break;

    case TYPE_STAR_S_BH:
    case TYPE_STAR_IM_BH:
    case TYPE_STAR_SM_BH:
        gi = InstanceGenerator<TerrainHeightEllipsoid, TerrainColorBlack>;
        break;

    case TYPE_PLANET_GAS_GIANT:
    {
        const GeneratorInstancer choices[] = {
            InstanceGenerator<TerrainHeightFlat, TerrainColorGGJupiter>,
            InstanceGenerator<TerrainHeightFlat, TerrainColorGGSaturn>,
            InstanceGenerator<TerrainHeightFlat, TerrainColorGGSaturn2>,
            InstanceGenerator<TerrainHeightFlat, TerrainColorGGNeptune>,
            InstanceGenerator<TerrainHeightFlat, TerrainColorGGNeptune2>,
            InstanceGenerator<TerrainHeightFlat, TerrainColorGGUranus>,
            InstanceGenerator<TerrainHeightFlat, TerrainColorGGSaturn>
        };
        gi = choices[rand.Int32(COUNTOF(choices))];
        break;
    }

    default:
        gi = InstanceGenerator<TerrainHeightFlat, TerrainColorWhite>;
        break;
    }

    return gi(body);
}



}
