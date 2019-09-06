#include "LevelManager.h"
#include "Levels/Splash.h"
#include "Levels/MainMenu.h"
#include "Levels/Level.h"
#include "Levels/ExitGame.h"
#include "Levels/Loading.h"
#include "Levels/Credits.h"

#include "game_data.h"
#include "workshop.h"
#include "one_planet.h"
#include "tech_tree.h"
#include "design_manager.h"
#include "camera_orb_2.h"
#include "pivot_marker.h"
#include "box.h"
#include "thruster.h"
#include "kepler_mover.h"
#include "launch_site.h"
#include "planet_forces.h"
#include "planet_test.h"
#include "planet_sun_test.h"
#include "planet_moon_test.h"
#include "planet_cs.h"
#include "player.h"

#include "physics_world_2.h"
#include "world_mover.h"

#include "MyEvents.h"


LevelManager::LevelManager(Context* context) :
Object(context)
{
    // Register all classes
    RegisterAllFactories();

    // Listen to set level event
    SubscribeToEvent(MyEvents::E_SET_LEVEL, URHO3D_HANDLER(LevelManager, HandleSetLevelQueue));

    if (GetSubsystem<UI>()) {
        GetSubsystem<UI>()->GetRoot()->RemoveAllChildren();
    }

    // How to use lambda (anonymous) functions
    SendEvent(MyEvents::E_CONSOLE_COMMAND_ADD, MyEvents::ConsoleCommandAdd::P_NAME, "change_level", MyEvents::ConsoleCommandAdd::P_EVENT, "ChangeLevelConsole", MyEvents::ConsoleCommandAdd::P_DESCRIPTION, "Change level");
    SubscribeToEvent("ChangeLevelConsole", [&](StringHash eventType, VariantMap& eventData) {
        StringVector params = eventData["Parameters"].GetStringVector();

        const Variant value = GetSubsystem<Engine>()->GetGlobalVar(params[0]);

        // Only show variable
        if (params.Size() != 2) {
            URHO3D_LOGERROR("Invalid number of parameters!");
        } else {
            VariantMap data = GetEventDataMap();
            data[MyEvents::SetLevel::P_NAME] = params[1];
            SendEvent(MyEvents::E_SET_LEVEL, data);
        }
    });
}

LevelManager::~LevelManager()
{
}


void LevelManager::RegisterAllFactories()
{
    // Register classes
    context_->RegisterFactory<Levels::Splash>();
    context_->RegisterFactory<Levels::Level>();
    context_->RegisterFactory<Levels::MainMenu>();
    context_->RegisterFactory<Levels::ExitGame>();
    context_->RegisterFactory<Levels::Loading>();
    context_->RegisterFactory<Levels::Credits>();

    context_->RegisterFactory<Osp::Workshop>();
    context_->RegisterFactory<Osp::OnePlanet>();

    // Registering objects.
    context_->RegisterFactory<Osp::GameData>();
    context_->RegisterSubsystem( new Osp::TechTree( context_ ) );
    context_->RegisterSubsystem( new Osp::DesignManager( context_ ) );
    context_->RegisterFactory<Osp::Assembly>();
    context_->RegisterFactory<Osp::Box>();
    context_->RegisterFactory<Osp::Thruster>();
    context_->RegisterFactory<Osp::PivotMarker>();
    context_->RegisterFactory<Osp::CameraOrb2>();
    context_->RegisterFactory<Osp::KeplerMover>();
    context_->RegisterFactory<Osp::KeplerRotator>();
    context_->RegisterFactory<Osp::LaunchSite>();
    context_->RegisterFactory<Osp::PlanetTest>();
    context_->RegisterFactory<Osp::PlanetSunTest>();
    context_->RegisterFactory<Osp::PlanetMoonTest>();
    context_->RegisterFactory<Osp::PlanetCs>();

    context_->RegisterFactory<Osp::Player>();
    context_->RegisterFactory<Osp::WorldMover>();
    context_->RegisterFactory<Osp::PlanetForces>();


    RegisterPhysicsLibrary2( context_ );
}

void LevelManager::HandleSetLevelQueue(StringHash eventType, VariantMap& eventData)
{
    // Busy now
    if (level_queue_.Size() == 0)
    {
        // Init fade status
        fade_status_ = 0;
    }

    // Push to queue
    const String & levelName = eventData["Name"].GetString();
    URHO3D_LOGINFO( "Opening level: " + levelName );
    level_queue_.Push( levelName );
    data_ = eventData;

    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(LevelManager, HandleUpdate));
}

void LevelManager::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move sprites, scale movement with time step
    fade_time_ -= timeStep;

    // Prepare to fade out
    if (fade_status_ == 0) {
        if ( level_ )
        {
            using namespace MyEvents::LevelDeactivating;
            VariantMap data = GetEventDataMap();
            data[P_FROM] = currentLevel_;
            data[P_TO] = level_queue_.Front();
            SendEvent(MyEvents::E_LEVEL_DEACTIVATING, data);
        }

        // No old level
        else
        {
            fade_status_++;
            return;
        }
        // Add a new fade layer
        AddFadeLayer();
        fade_window_->SetOpacity(0.0f);
        fade_time_ = MAX_FADE_TIME;
        fade_status_++;

        return;
    }

    // Fade out
    if (fade_status_ == 1) {
        // No old level
        if (!level_) {
            fade_status_++;
            return;
        }
        else
        {
            using namespace MyEvents::LevelFadeOut;
            VariantMap data = GetEventDataMap();
            data[P_FROM] = currentLevel_;
            data[P_TO] = level_queue_.Front();
            SendEvent(MyEvents::E_LEVEL_FADE_OUT, data);
        }
        fade_window_->SetFocus(true);
        fade_window_->SetOpacity(1.0f - fade_time_ / MAX_FADE_TIME);

        // Increase fade status
        if (fade_time_ <= 0.0f) {
            fade_status_++;
        }
        return;
    }

    // Release old level
    if (fade_status_ == 2)
    {
        if ( level_ )
        {
            using namespace MyEvents::LevelDeactivated;
            VariantMap data = GetEventDataMap();
            data[P_FROM] = currentLevel_;
            data[P_TO] = level_queue_.Front();
            SendEvent( MyEvents::E_LEVEL_DEACTIVATED, data );

            // We can not create new level here, or it may cause errors, we have to create it at the next update point.
            level_ = SharedPtr<Object>();
        }
        fade_status_++;

        // Send event to close all active UI windows
        SendEvent( MyEvents::E_CLOSE_ALL_WINDOWS );
        return;
    }

    // Create new level
    if (fade_status_ == 3)
    {
        // Create new level
        const String & levelName = level_queue_.Front();
        URHO3D_LOGINFO( "Creating level: " + levelName );
        level_ = context_->CreateObject( StringHash(levelName) );
        if (!level_) {
            URHO3D_LOGERROR("Level '" + level_queue_.Front() + "' doesn't exist in the system! Moving to 'Splash' level");

            auto* localization = GetSubsystem<Localization>();
            VariantMap& eventData = GetEventDataMap();
            eventData["Name"] = "MainMenu";
            eventData["Message"] = localization->Get("LEVEL_NOT_EXIST") + " :" + level_queue_.Front();
            SendEvent(MyEvents::E_SET_LEVEL, eventData);

            level_queue_.PopFront();
            return;
        }
        SendEvent( MyEvents::E_LEVEL_ACTIVATING, data_ );

        previousLevel_ = currentLevel_;
        currentLevel_ = level_queue_.Front();
        SetGlobalVar("CurrentLevel", currentLevel_);

        GetSubsystem<DebugHud>()->SetAppStats("Current level", currentLevel_);

        // Add a new fade layer
        AddFadeLayer();
        fade_window_->SetOpacity(1.0f);
        fade_time_ = MAX_FADE_TIME;
        fade_status_++;

        return;
    }

    // Fade in
    if (fade_status_ == 4)
    {
        fade_window_->SetFocus(true);
        fade_window_->SetOpacity(fade_time_ / MAX_FADE_TIME);

        // Increase fade status
        if (fade_time_ <= 0.0f) {
            fade_status_++;
        }

        using namespace MyEvents::LevelFadeIn;
        VariantMap data = GetEventDataMap();
        data[P_FROM] = previousLevel_;
        data[P_TO] = currentLevel_;
        SendEvent(MyEvents::E_LEVEL_FADE_IN, data);

        return;
    }

    // Finished
    if (fade_status_ == 5)
    {
        // Remove fade layer
        fade_window_->Remove();
        fade_window_.Reset();

        {
            using namespace MyEvents::LevelActivated;
            VariantMap data = GetEventDataMap();
            data[P_FROM] = previousLevel_;
            data[P_TO] = level_queue_.Front();
            SendEvent(MyEvents::E_LEVEL_ACTIVATED, data);
        }

        VariantMap data = GetEventDataMap();
        data["Name"] = level_queue_.Front();
        SendEvent("LevelLoaded", data);

        // Remove the task
        level_queue_.PopFront();

        // Release all unused resources
        GetSubsystem<ResourceCache>()->ReleaseAllResources(false);

        if ( level_queue_.Size() != 0 )
        {
            // Subscribe HandleUpdate() function for processing update events
            SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(LevelManager, HandleUpdate));

            // // Init fade status
            fade_status_ = 0;
        }
        else
        {
            // Unsubscribe update event
            UnsubscribeFromEvent(E_UPDATE);
        }
        return;
    }
}

void LevelManager::AddFadeLayer()
{
    if (fade_window_)
    {
        fade_window_->Remove();
        fade_window_.Reset();
    }
    fade_window_ = new Window(context_);
    // Make the window a child of the root element, which fills the whole screen.
    GetSubsystem<UI>()->GetRoot()->AddChild(fade_window_);
    fade_window_->SetSize(GetSubsystem<Graphics>()->GetWidth(), GetSubsystem<Graphics>()->GetHeight());
    fade_window_->SetLayout(LM_FREE);
    // Urho has three layouts: LM_FREE, LM_HORIZONTAL and LM_VERTICAL.
    // In LM_FREE the child elements of this window can be arranged freely.
    // In the other two they are arranged as a horizontal or vertical list.

    // Center this window in it's parent element.
    fade_window_->SetAlignment(HA_CENTER, VA_CENTER);
    // Black color
    fade_window_->SetColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
    // Make it topmost
    fade_window_->BringToFront();
    fade_window_->SetPriority(1000);
}
