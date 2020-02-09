

#ifndef __PI_BODY_H_
#define __PI_BODY_H_


#include "data_types.h"
#include "cube_sphere.h"
#include "pi_fract_def.h"
#include "pi_random.h"

namespace Ign
{

enum BodyType
{
    GAS_GIANT = 0, 
    PLANET_ASTEROID, 
    PLANET_TERRESTRIAL
};


struct PiBodyDesc
{
    int   seed_;
    Float GM_;
    int   life_;
    int   gas_;
    int   average_temp_;
    int   liquid_;
    int   volcanic_;
    int   ice_;
    int   metal_;

    PiBodyDesc();

    static const int MAX_VALUE;
};

class PiBodySource: public HeightSource
{
public:
    PiBodySource();
    virtual ~PiBodySource();

    void operator=( const PiBodyDesc & desc );




   int m_seed;
   PiRandom m_rand;

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



