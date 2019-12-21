
#include "3dparty_register.h"
#include "ConfigFile.h"
#include "ConfigManager.h"
#include "Notifications.h"

namespace Ign
{
    void Register3dparty( Context * context )
    {
        ConfigFile::RegisterObject( context );
        ConfigManager::RegisterObject( context );
        context->RegisterSubsystem<Notifications>();
    }
}

