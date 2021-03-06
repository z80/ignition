#pragma once

#include <Urho3D/Engine/Console.h>
#include <Urho3D/Container/Str.h>

using namespace Urho3D;

struct SingleConsoleCommand
{
    String     command;
    StringHash eventToCall;
    String     description;
};

class ConsoleHandler : public Object
{
    URHO3D_OBJECT( ConsoleHandler, Object )

public:
    static bool AddCommand( Context * context, const SingleConsoleCommand & commandDesc );

    /// Construct.
    ConsoleHandler(Context* context);

    virtual ~ConsoleHandler();

    virtual void Init();

    /**
     * Create the console
     */
    virtual void Create();

private:

    /**
     * Subscribe console related events
     */
    void SubscribeToEvents();

    /**
     * Toggle console
     */
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);

    /**
     * Add new console command
     */
    bool ConsoleCommandAdd( const String & command, const StringHash & eventToCall, const String & description=String(), bool overwrite=false );
    void HandleConsoleCommandAdd(StringHash eventType, VariantMap& eventData);

    /**
     * Process incomming console commands
     */
    void HandleConsoleCommand(StringHash eventType, VariantMap& eventData);

    /**
     * Parse incomming command input
     */
    void ParseCommand( const String & input);

    /**
     * Display help
     */
    void HandleConsoleCommandHelp(StringHash eventType, VariantMap& eventData);

    /**
     * Handle configuration change via console
     */
    void HandleConsoleGlobalVariableChange(StringHash eventType, VariantMap& eventData);

    /**
     * Registered console commands
     */
    HashMap<String, SingleConsoleCommand> _registeredConsoleCommands;

    /**
     * Console handler in the engine
     */
    SharedPtr<Console> _console;
};