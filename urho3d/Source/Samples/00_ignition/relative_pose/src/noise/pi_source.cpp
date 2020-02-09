
#define NOMINMAX

#include "pi_source.h"
#include <algorithm>

static  Float Clamp( Float v, Float vmin, Float vmax )
{
	if ( v < vmin )
		return vmin;
	if ( v > vmax )
		return vmax;
	return v;
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

	type_ = PLANET_TERRESTRIAL;

    GM_ = 1.0;
	radius_ = 30.0;

    life_ = 0.1;
    gas_ = 0.05;
    average_temp_ = 240.0;
    liquid_ = 0.5;
    volcanic_ = 0.2;
    ice_ = 0.2;
    metal_ = 0.1;
}

PiBodySource::PiBodySource( const PiSourceDesc & body )
    : HeightSource(), 
	m_seed(body.seed_),
	m_rand(body.seed_),
	//m_heightScaling(0),
	m_minh(0)

{
	const Float m_heightScaling = 1.0;

	m_minBody = body;



	m_sealevel = Clamp(body.liquid_, 0.0, 1.0);
	m_icyness = Clamp(body.ice_, 0.0, 1.0);
	m_volcanic = Clamp(body.volcanic_, 0.0, 1.0); // height scales with volcanicity as well
	m_surfaceEffects = 0;

	const double rad = m_minBody.radius_;

	// calculate max height
	// max mountain height for earth-like planet (same mass, radius)
	m_maxHeightInMeters = std::max(100.0, (9000.0 * rad * rad * (m_volcanic + 0.5)) / (body.GM_));
	m_maxHeightInMeters = std::min(rad, m_maxHeightInMeters); // small asteroid case

	// and then in sphere normalized jizz
	m_maxHeight = m_maxHeightInMeters / rad;
	//Output("%s: max terrain height: %fm [%f]\n", m_minBody.name.c_str(), m_maxHeightInMeters, m_maxHeight);
	m_invMaxHeight = 1.0 / m_maxHeight;
	m_planetRadius = rad;
	m_invPlanetRadius = 1.0 / rad;
	m_planetEarthRadii = rad / 1.0; //EARTH_RADIUS;

	// Pick some colors, mainly reds and greens
	for (int i = 0; i < int(COUNTOF(m_entropy)); i++)
		m_entropy[i] = m_rand.Double();
	for (int i = 0; i < int(COUNTOF(m_rockColor)); i++)
	{
		double r, g, b;
		r = m_rand.Double(0.3, 1.0);
		g = m_rand.Double(0.3, r);
		b = m_rand.Double(0.3, g);
		r = std::max(b, r * body.metal_);
		g = std::max(b, g * body.metal_);
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
		r = std::max(b, r * body.metal_);
		g = std::max(b, g * body.metal_);
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
		g = std::max(r, g * body.life_);
		b *= (1.0 - body.life_);
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
		g = std::max(r, g * body.life_);
		b *= (1.0 - body.life_);
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
    const Vector3d c = GetColor(at, 0.0, Vector3d::ZERO);
    const Color col(c.x_, c.y_, c.z_, 1.0);
    return col;
}





































//This is the most complex and insanely crazy terrain you will ever see :
class TerrainHeightFlat;

//New terrains with less noise :
class TerrainHeightAsteroid;
class TerrainHeightAsteroid2;
class TerrainHeightAsteroid3;
class TerrainHeightAsteroid4;
class TerrainHeightBarrenRock;
class TerrainHeightBarrenRock2;
class TerrainHeightBarrenRock3;
/* Pictures of the above terrains:
http://i.imgur.com/cJO4E.jpg
http://i.imgur.com/BtB0g.png
http://i.imgur.com/qeEuS.png
*/

class TerrainHeightEllipsoid;

// Newish terrains, 6 months or so :
class TerrainHeightHillsCraters2;
class TerrainHeightHillsCraters;
class TerrainHeightHillsDunes;
//   This terrain or the following one should have terragen style ridged mountains :
//   (As seen in an ancient clip of Mars http://www.youtube.com/watch?v=WeO28VBTWxs )
class TerrainHeightHillsNormal;
class TerrainHeightHillsRidged;
class TerrainHeightHillsRivers;

class TerrainHeightMapped;
class TerrainHeightMapped2;
class TerrainHeightMountainsCraters2;
class TerrainHeightMountainsCraters;

//Probably the best looking terrain due to variety, but among the most costly too :
//(It was also used for mars at some point : http://www.youtube.com/watch?feature=player_embedded&v=4-DcyQm0zE4 )
/// and http://www.youtube.com/watch?v=gPtxUUunSWg&t=5m15s
class TerrainHeightMountainsNormal;
// Based on TerrainHeightMountainsNormal :
class TerrainHeightMountainsRivers;
/*Pictures from the above two terrains generating Earth-like worlds:
http://www.spacesimcentral.com/forum/download/file.php?id=1533&mode=view
http://www.spacesimcentral.com/forum/download/file.php?id=1544&mode=view
http://www.spacesimcentral.com/forum/download/file.php?id=1550&mode=view
http://www.spacesimcentral.com/forum/download/file.php?id=1540&mode=view
*/

// Older terrains:
class TerrainHeightMountainsRidged;
class TerrainHeightMountainsRiversVolcano;
//   Used to be used for mars since it has a megavolcano:
class TerrainHeightMountainsVolcano;

//Oldest terrains, from before fracdefs :
class TerrainHeightRuggedDesert;
//   lava terrain should look like this http://www.spacesimcentral.com/forum/download/file.php?id=1778&mode=view
class TerrainHeightRuggedLava;

/*Terrains used for Iceworlds,
only terrain to use the much neglected impact crater function
(basically I forgot about it;) ) **It makes cool looking sunken craters** */
class TerrainHeightWaterSolidCanyons;
class TerrainHeightWaterSolid;

class TerrainColorAsteroid;
class TerrainColorBandedRock;
class TerrainColorBlack;
class TerrainColorDeadWithWater;
class TerrainColorDesert;
/*ColorEarthlike uses features not yet included in all terrain colours
such as better poles : http://www.spacesimcentral.com/forum/download/file.php?id=1884&mode=view
http://www.spacesimcentral.com/forum/download/file.php?id=1885&mode=view
and better distribution of snow :  http://www.spacesimcentral.com/forum/download/file.php?id=1879&mode=view  */
class TerrainColorEarthLike;
class TerrainColorEarthLikeHeightmapped;
class TerrainColorGGJupiter;
class TerrainColorGGNeptune2;
class TerrainColorGGNeptune;
class TerrainColorGGSaturn2;
class TerrainColorGGSaturn;
class TerrainColorGGUranus;
class TerrainColorIce;
class TerrainColorMethane;
class TerrainColorRock2;
class TerrainColorRock;
class TerrainColorWhite;
class TerrainColorStarBrownDwarf;
class TerrainColorStarG;
class TerrainColorStarK;
class TerrainColorStarM;
class TerrainColorStarWhiteDwarf;
class TerrainColorTFGood;
class TerrainColorTFPoor;
class TerrainColorVolcanic;



PiBodySource * PiBodySource::InstanceTerrain( const PiSourceDesc & body )
{
	PiRandom rand(body.seed_);

	GeneratorInstancer gi = nullptr;

	switch ( body.type_ )
	{


	case GAS_GIANT: {
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

	case PLANET_ASTEROID: {
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
		gi = choices[rand.Int32(COUNTOF(choices))];
		break;
	}

	case PLANET_TERRESTRIAL: {

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
		};

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



}
