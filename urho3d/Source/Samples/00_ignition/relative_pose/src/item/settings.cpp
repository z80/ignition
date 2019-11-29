
#include "settings.h"

namespace Ign
{

Settings::Settings()
{}

Settings::~Settings()
{}

Timestamp Settings::ticks( Float secs_dt )
{
    const Float t = secs_dt * static_cast<Float>( 1000 );
    const Timestamp dt = static_cast<Timestamp>( t );
    return dt;
}

Float Settings::secs( Timestamp ticks_dt )
{
    const Float dt = static_cast<Float>( ticks_dt ) / static_cast<Float>( 1000 );
    return dt;
}

Float Settings::maxDynamicsTimeStep()
{
    return 0.1;
}

Float Settings::absLimitDynamicsTimeStep()
{
    return 0.5;
}

Timestamp Settings::maxEvolutionTimeStep()
{
    // 10 minutes
    return 60*10*1000;
}

Float Settings::teleportDistance()
{
    return 100.0;
}

Float Settings::staticObjDistanceHorizontShow()
{
    return 500.0;
}

Float Settings::staticObjDistanceHorizontHide()
{
    return 600.0;
}

Float Settings::dynamicsWorldDistanceInclude()
{
    return 400.0;
}

Float Settings::dynamicsWorldDistanceExclude()
{
    return 500.0;
}


}


