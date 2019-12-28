
#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include <Urho3D/Urho3DAll.h>
#include "data_types.h"

using namespace Urho3D;

class ConfigManager;

namespace Ign
{

class Settings: public Object
{
    URHO3D_OBJECT( Settings, Object )
public:
    void RegisterObject( Context * context );

    Settings( Context * context );
    ~Settings();

    static Timestamp ticks( Float secs_dt );
    static Float     secs( Timestamp ticks_dt );
    static Float     maxDynamicsTimeStep();
    static Float     absLimitDynamicsTimeStep();
    static Timestamp maxEvolutionTimeStep();

    /// If bigger than this teleport dynamics world.
    static Float teleportDistance();

    /// If less than this, show the object.
    static Float staticObjDistanceHorizontShow();
    /// If smaller than this, hide the object.
    static Float staticObjDistanceHorizontHide();

    /// If closer include into dynamics world.
    static Float dynamicsWorldDistanceInclude();
    /// If further away, exclude from dynamics world.
    static Float dynamicsWorldDistanceExclude();

    static Float cameraMaxDistance();
    static Float cameraMinDistance();

private:
    void LoadConfig();
    void SaveConfig();

    ConfigManager * configManager();

    VariantMap settings_;
    static const String configFileName_;
};

}



#endif

