
#ifndef __DETERMINISTIC_SOURCE_0_H_
#define __DETERMINISTIC_SOURCE_0_H_

#include "pi_source.h"
#include "cube_sphere.h"
#include "pi_fract_def.h"
#include "pi_random.h"

namespace Ign
{

class DeterministicSource0: public HeightSource
{
public:
    DeterministicSource0( const PiSourceDesc & body );
    ~DeterministicSource0();

    Float height( const Vector3d & at ) const override;
    Color color( const Vector3d & at )  const override;

public:
    void setFracDef(const unsigned int index, const Float featureHeightMeters, const Float featureWidthMeters, const Float smallestOctaveMeters = 20.0);
    inline const fracdef_t & getFracDef( const unsigned int index ) const
    {
        assert(index >= 0 && index < MAX_FRACDEFS);
        return m_fracdef[index];
    }

    PiSourceDesc src_;

    int      m_seed;
    PiRandom m_rand;

    Float m_heightScaling;

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

}

#endif



