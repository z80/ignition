

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



}


#endif



