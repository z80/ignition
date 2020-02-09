

#ifndef __PI_BODY_H_
#define __PI_BODY_H_


#include "data_types.h"
#include "cube_sphere.h"
#include "pi_fract_def.h"
#include "pi_random.h"

namespace Ign
{

template <typename, typename> class TerrainGenerator;


enum BodyType
{
    GAS_GIANT = 0, 
    PLANET_ASTEROID, 
    PLANET_TERRESTRIAL
};


struct PiSourceDesc
{
    int   seed_;
	BodyType type_;
    Float GM_;
	Float radius_;
    Float life_;
    Float gas_;
    Float average_temp_;
    Float liquid_;
    Float volcanic_;
    Float ice_;
    Float metal_;

    PiSourceDesc();
};

class PiBodySource: public HeightSource
{
public:
    static PiBodySource * InstanceTerrain( const PiSourceDesc & body );


    PiBodySource( const PiSourceDesc & body );
    virtual ~PiBodySource();

	Float height( const Vector3d & at ) const override;
	Color color( const Vector3d & at ) const override;


	virtual Float GetHeight( const Vector3d & p ) const = 0;
	virtual Vector3d GetColor(const Vector3d & p, double height, const Vector3d & norm ) const = 0;

	virtual const char *GetHeightFractalName() const = 0;
	virtual const char *GetColorFractalName() const = 0;


    int      m_seed;
    PiRandom m_rand;
	PiSourceDesc m_minBody;

    Float m_sealevel; // 0 - no water, 1 - 100% coverage
    Float m_icyness; // 0 - 1 (0% to 100% cover)
    Float m_volcanic;

    int m_surfaceEffects;

    // heightmap stuff
    // XXX unify heightmap types
    Float m_minh;


    /** General attributes */
    Float m_maxHeight;
    Float m_maxHeightInMeters;
    Float m_invMaxHeight;
    Float m_planetRadius;
    Float m_invPlanetRadius;
    Float m_planetEarthRadii;

    Float m_entropy[12];

    Vector3d m_rockColor[8];
    Vector3d m_darkrockColor[8];
    Vector3d m_greyrockColor[8];
    Vector3d m_plantColor[8];
    Vector3d m_darkplantColor[8];
    Vector3d m_sandColor[8];
    Vector3d m_darksandColor[8];
    Vector3d m_dirtColor[8];
    Vector3d m_darkdirtColor[8];
    Vector3d m_gglightColor[8];
    Vector3d m_ggdarkColor[8];

    /* XXX you probably shouldn't increase this. If you are
       using more than 10 then things will be slow as hell */
    static const int MAX_FRACDEFS = 10;
    fracdef_t m_fracdef[MAX_FRACDEFS];
};







template <typename HeightFractal>
class TerrainHeightFractal : virtual public PiBodySource
{
public:
	TerrainHeightFractal() = delete;
	virtual Float GetHeight(const Vector3d & p) const;
	virtual const char *GetHeightFractalName() const;

protected:
	TerrainHeightFractal(const PiSourceDesc & body);

private:
};

template <typename ColorFractal>
class TerrainColorFractal : virtual public PiBodySource
{
public:
	TerrainColorFractal() = delete;
	virtual Vector3d GetColor( const Vector3d & p, Float height, const Vector3d & norm ) const;
	virtual const char *GetColorFractalName() const;

protected:
	TerrainColorFractal( const PiSourceDesc & body );

private:
};

template <typename HeightFractal, typename ColorFractal>
class TerrainGenerator : public TerrainHeightFractal<HeightFractal>, public TerrainColorFractal<ColorFractal>
{
public:
	TerrainGenerator() = delete;
	TerrainGenerator(const PiSourceDesc & body ):
		PiBodySource(body),
		TerrainHeightFractal<HeightFractal>(body),
		TerrainColorFractal<ColorFractal>(body) {}

private:
};










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




}


#endif



