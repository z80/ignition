
#ifndef __SETTINGS_H_
#define __SETTINGS_H_

#include <Urho3D/Urho3DAll.h>
#include "data_types.h"

namespace Ign
{

class Settings
{
public:
    Settings();
    ~Settings();

    static Timestamp ticks( Float secs_dt ) const;
    static Float     secs( Timestamp ticks_dt ) const;
    static Float     maxDynamicsTimeStep() const;
    static Float     absLimitDynamicsTimeStep() const;
    static Timestamp maxEvolutionTimeStep() const;

    /// If less than this, show the object.
    static Float staticObjDistanceHorizontShow();
    /// If smaller than this, hide the object.
    static Float staticObjDistanceHorizontHide();

    /// If closer include into dynamics world.
    static Float dynamicsWorldDistanceInclude();
    /// If further away, exclude from dynamics world.
    static Float dynamicsWorldDistanceExclude();
};

}



#endif

