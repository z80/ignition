#pragma once
#include <Urho3D/Urho3DAll.h>

namespace MyEvents
{
    // Start new level
    URHO3D_EVENT(E_SET_LEVEL, SetLevel)
    {
        URHO3D_PARAM(P_NAME, Name); // string - level object name
        URHO3D_PARAM(P_MESSAGE, Message); // string - pop-up message content, can be empty
    }

    // Open UI Window
    URHO3D_EVENT(E_OPEN_WINDOW, OpenWindow)
    {
        URHO3D_PARAM(P_NAME, Name); // string - window object name
		URHO3D_PARAM(P_CLOSE_PREVIOUS, ClosePrevious); // bool - Close window first if it was already opened
    }
    // Close UI Window
    URHO3D_EVENT(E_CLOSE_WINDOW, CloseWindow)
    {
        URHO3D_PARAM(P_NAME, Name); // string - window object name
    }
    // When specific UI window is closed
    URHO3D_EVENT(E_WINDOW_CLOSED, WindowClosed)
    {
        URHO3D_PARAM(P_NAME, Name); // string - close window object name
    }
    // Close all active UI Windows
    URHO3D_EVENT(E_CLOSE_ALL_WINDOWS, CloseAllWindows)
    {
    }

    // Add new console command
	URHO3D_EVENT(E_CONSOLE_COMMAND_ADD, ConsoleCommandAdd)
	{
		URHO3D_PARAM(P_NAME, ConsoleCommandName); // string - command name
		URHO3D_PARAM(P_EVENT, ConsoleCommandEvent); // string - event to call when command entered
		URHO3D_PARAM(P_DESCRIPTION, ConsoleCommandDescription); // string - short description of the command
	}

    // Change global system variable
	URHO3D_EVENT(E_CONSOLE_GLOBAL_VARIABLE_CHANGE, ConsoleGlobalVariableChange)
	{
		URHO3D_PARAM(P_NAME, GlobalVariableName); // string - global variable name
		URHO3D_PARAM(P_VALUE, GlobalVariableValue); // string - new value
	}

    // When global variable is changed
	URHO3D_EVENT(E_CONSOLE_GLOBAL_VARIABLE_CHANGED, ConsoleGlobalVariableChanged)
	{
		URHO3D_PARAM(P_NAME, GlobalVariableName); // string - global variable name
		URHO3D_PARAM(P_VALUE, GlobalVariableValue); // string - new value
	}

    // Add new global variable which should be saved in the config files
    URHO3D_EVENT(E_ADD_CONFIG, AddConfig)
    {
        URHO3D_PARAM(P_NAME, Name); // string - global variable name
    }

    // Load configuration file
    URHO3D_EVENT(E_LOAD_CONFIG, LoadConfig)
    {
        URHO3D_PARAM(P_FILEPATH, Filepath); // string - filepath + filename, relative to executable
        URHO3D_PARAM(P_PREFIX, Prefix); // string - prefix, which will be added to loaded configuration variables, can be empty
    }

    // Start mapping key to specific action
	URHO3D_EVENT(E_START_INPUT_MAPPING, StartInputMapping)
	{
		URHO3D_PARAM(P_CONTROL_ACTION, ControlAction); // string or int - action name or ID
	}

	// Start mapping key to specific action
	URHO3D_EVENT(E_STOP_INPUT_MAPPING, StopInputMapping)
	{
		URHO3D_PARAM(P_CONTROL_ACTION, ControlAction); // string or int - action name or ID
	}

    // When mapping was finished
    URHO3D_EVENT(E_INPUT_MAPPING_FINISHED, InputMappingFinished)
	{
        URHO3D_PARAM(P_CONTROLLER, Controller); // string - keyboard, mouse, joystick - which controller was used to do the mapping
		URHO3D_PARAM(P_CONTROL_ACTION, ControlAction); // int - action ID
        URHO3D_PARAM(P_ACTION_NAME, ActionName); // string - action name
        URHO3D_PARAM(P_KEY, Key); // int - key ID, relative to P_CONTROLLEr
        URHO3D_PARAM(P_KEY_NAME, KeyName); // string - key name
	}

	// New controller/joystick added
	URHO3D_EVENT(E_CONTROLLER_ADDED, ControllerAdded)
	{
		URHO3D_PARAM(P_INDEX, Index); // string or int - controller id
	}

	// controller/joystick removed
	URHO3D_EVENT(E_CONTROLLER_REMOVED, ControllerRemoved)
	{
		URHO3D_PARAM(P_INDEX, Index); // string or int - controller id
	}

    // play sound
	URHO3D_EVENT(E_PLAY_SOUND, PlaySound)
	{
		URHO3D_PARAM(P_INDEX, Index); // string or int - sound id
        URHO3D_PARAM(P_TYPE, Type); // string - sound type - Master / Effect / Ambient / Voice / Music
        URHO3D_PARAM(P_SOUND_FILE, SoundFile); // string - full path to sound file, not needed when Index is used
	}

    // stop sound
	URHO3D_EVENT(E_STOP_SOUND, StopSound)
	{
		URHO3D_PARAM(P_INDEX, Index); // int - sound id
        URHO3D_PARAM(P_TYPE, Type); // string - sound type - Master / Effect / Ambient / Voice / Music
	}

    // stop all sounds in progress
	URHO3D_EVENT(E_STOP_ALL_SOUNDS, StopAllSounds)
	{
	}

    URHO3D_EVENT(E_LEVEL_DEACTIVATING, LevelDeactivating)
    {
        URHO3D_PARAM(P_FROM, From); // string
        URHO3D_PARAM(P_TO, To); // string
    }

    URHO3D_EVENT(E_LEVEL_FADE_OUT, LevelFadeOut)
    {
        URHO3D_PARAM(P_FROM, From); // string
        URHO3D_PARAM(P_TO, To); // string
    }

    URHO3D_EVENT(E_LEVEL_DEACTIVATED, LevelDeactivated )
    {
        URHO3D_PARAM(P_FROM, From); // string
        URHO3D_PARAM(P_TO, To); // string
    }

    // Level changing started
    URHO3D_EVENT( E_LEVEL_ACTIVATING, LevelActivating )
	{
        URHO3D_PARAM(P_FROM, From); // string
        URHO3D_PARAM(P_TO, To); // string
	}

    // When the new level is actually created, before the fade effect goes away
    URHO3D_EVENT(E_LEVEL_FADE_IN, LevelFadeIn)
    {
        URHO3D_PARAM(P_FROM, From); // string
        URHO3D_PARAM(P_TO, To); // string
    }

    // Level changing finished
    URHO3D_EVENT(E_LEVEL_ACTIVATED, LevelActivated)
	{
        URHO3D_PARAM(P_FROM, From); // string - previous level
        URHO3D_PARAM(P_TO, To); // string - new level
	}

	// Level changing finished
	URHO3D_EVENT(E_NEW_ACHIEVEMENT, NewAchievement)
	{
		URHO3D_PARAM(P_MESSAGE, Message); // string - achievement title
		URHO3D_PARAM(P_IMAGE, Image); // string - Texture to use for achievement
	}

	// Achievement has been unlocked
	URHO3D_EVENT(E_ACHIEVEMENT_UNLOCKED, AchievementUnlocked)
	{
		URHO3D_PARAM(P_MESSAGE, Message); // string - achievement title
	}

    // Register new achievement
    URHO3D_EVENT(E_ADD_ACHIEVEMENT, AddAchievement)
    {
        URHO3D_PARAM(P_EVENT, Event); // string - achievement event
        URHO3D_PARAM(P_MESSAGE, Message); // string - achievement event
        URHO3D_PARAM(P_IMAGE, Image); // string - achievement event
        URHO3D_PARAM(P_THRESHOLD, Threshold); // string - achievement event
        URHO3D_PARAM(P_PARAMETER_NAME, ParameterName); // string - achievement event
        URHO3D_PARAM(P_PARAMETER_VALUE, ParameterValue); // string - achievement event
    }

    // Video settings changed event
    URHO3D_EVENT(E_VIDEO_SETTINGS_CHANGED, VideoSettingsChanged)
    {
    }

    // Video settings changed event
    URHO3D_EVENT(E_PLAYER_SCORES_UPDATED, PlayerScoresUpdated)
    {
    }

	// Register new loading step in the loading screen
	URHO3D_EVENT(E_REGISTER_LOADING_STEP, RegisterLoadingStep)
	{
		URHO3D_PARAM(P_EVENT, Event); // string - event to call to start loading process
		URHO3D_PARAM(P_NAME, Name); // string - name of the loading step
	}

	// ACK event to mark loading step as valid
	URHO3D_EVENT(E_ACK_LOADING_STEP, AckLoadingStep)
	{
		URHO3D_PARAM(P_EVENT, Event); // string - event to call to start loading process
	}

	// Loading step progress update
    URHO3D_EVENT(E_LOADING_STEP_PROGRESS, LoadingStepProgress)
    {
        URHO3D_PARAM(P_EVENT, Event); // string - event to call to start loading process
        URHO3D_PARAM(P_PROGRESS, Progress); // float - 0.0 - 1.0 to indicate the loading step progress
    }

    // Load step loading finished event
	URHO3D_EVENT(E_LOADING_STEP_FINISHED, LoadingStepFinished)
	{
		URHO3D_PARAM(P_EVENT, Event); // string - event to call to start loading process
	}

	// Display pop-up message
	URHO3D_EVENT(E_SHOW_MESSAGE, ShowMessage)
	{
		URHO3D_PARAM(P_TITLE, Title); // string - Title of the message
		URHO3D_PARAM(P_MESSAGE, Message); // string - message content
		URHO3D_PARAM(P_TYPE, Type); // string - info / warning
	}




    // My events for game control.

    // When player selectes an assembly world mover need to position physics world
    // close to this assembly.
    // Sent by player.
    URHO3D_EVENT( E_ASSEMBLY_SELECTED, AssemblySelected )
    {
        URHO3D_PARAM( P_ASSEMBLY, Assembly );
    }

    // When planet of influence changes WorldMover sends this event
    // Sent by WorldMover.
    URHO3D_EVENT( E_WORLD_PLANET_CHANGED, WorldPlanetChanged )
    {
        URHO3D_PARAM( P_PLANET, Planet );
    }

    // When world is displaced all assemblies need to adjust their
    // positions and velocities if they are in physics world.
    // All objects in the world need to add "dr" to their positions
    // and "dv" to their velocities.
    // Sent by WorldMover.
    URHO3D_EVENT( E_WORLD_ADJUSTED, WorldAdjusted )
    {
        URHO3D_PARAM( P_POS_ADJ,    PositionAdj );
        URHO3D_PARAM( P_VEL_ADJ,    VelocityAdj );
        URHO3D_PARAM( P_ORBITING,   Orbiting );
    }
}
