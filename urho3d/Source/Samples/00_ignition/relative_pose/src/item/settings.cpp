
#include "settings.h"
#include "ConfigManager.h"

namespace Ign
{

static const Timestamp TICKS_PER_SEC = 1000;

const String Settings::configFileName_ = "config.json";

void Settings::RegisterObject( Context * context )
{
    context->RegisterFactory<Settings>();
}

Settings::Settings( Context * context )
    : Object( context )
{}

Settings::~Settings()
{}

Timestamp Settings::ticksPerSec()
{
    return TICKS_PER_SEC;
}

Timestamp Settings::ticks( Float secs_dt )
{
    const Float t = secs_dt * static_cast<Float>( TICKS_PER_SEC );
    const Timestamp dt = static_cast<Timestamp>( t );
    return dt;
}

Float Settings::secs( Timestamp ticks_dt )
{
    const Float dt = static_cast<Float>( ticks_dt ) / static_cast<Float>( TICKS_PER_SEC );
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
    return 10.0;
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

Float Settings::cameraMaxDistance()
{
    return 3000.0;
}

Float Settings::cameraMinDistance()
{
    return 1.0;
}

void Settings::LoadConfig()
{
    ConfigManager * m = configManager();
    const bool res = m->Load( configFileName_, true );
}

void Settings::SaveConfig()
{
    ConfigManager * m = configManager();
    const bool res = m->Save( configFileName_, true );
}

ConfigManager * Settings::configManager()
{
    Context * c = GetContext();
    ConfigManager * mgr = c->GetSubsystem<ConfigManager>();
    if ( !mgr )
        mgr = c->RegisterSubsystem<ConfigManager>();
    return mgr;
}



}


