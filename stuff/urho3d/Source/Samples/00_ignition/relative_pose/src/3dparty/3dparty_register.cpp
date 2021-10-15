
#include "3dparty_register.h"
#include "ConfigFile.h"
#include "ConfigManager.h"
#include "Notifications.h"
#include "AudioManager.h"
#include "WindowManager.h"
#include "ConsoleHandler.h"
#include "ControllerInput.h"

namespace Ign
{
    void Register3dparty( Context * context )
    {
        ConfigFile::RegisterObject( context );
        ConfigManager::RegisterObject( context );
        context->RegisterSubsystem<Notifications>();
        context->RegisterSubsystem<AudioManager>();
        context->RegisterSubsystem<WindowManager>();
        context->RegisterSubsystem<ConsoleHandler>();
        context->RegisterSubsystem<ControllerInput>();
    }
}

