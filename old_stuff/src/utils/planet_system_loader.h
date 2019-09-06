
#ifndef __PLANET_SYSTEM_LOADER_H_
#define __PLANET_SYSTEM_LOADER_H_

#include "planet_base.h"
#include "Urho3D/Urho3DAll.h"


namespace Osp
{

class GameData;
class PlanetCs;
class PlanetBase;

class PlanetSystemLoader
{
public:
    PlanetSystemLoader() {}
    ~PlanetSystemLoader() {}

    static bool load( GameData * gd, Scene * s );
    static bool loadPlanet( const String & name, PlanetBase * parent, const JSONValue & v, GameData * gd, Scene * s );
};

class PlanetLoader
{
public:
    PlanetLoader() {}
    ~PlanetLoader() {}

    static bool loadGeometry( const JSONValue & v, PlanetCs * p );
    static bool loadKepler(const JSONValue & v, PlanetBase *p );
    static bool loadRotator( const JSONValue & v, PlanetBase * p );
    static bool loadAssets( const String & fileName, PlanetCs * planet );
    static bool loadAsset( const JSONValue & v, PlanetCs * planet );
};

}



#endif


