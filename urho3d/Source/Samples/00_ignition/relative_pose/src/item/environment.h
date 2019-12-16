
#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include "Urho3D/Urho3DAll.h"
#include "data_types.h"

using namespace Urho3D;

namespace Ign
{

class RefFrame;

class ClientDesc
{
public:
    ClientDesc();
    ClientDesc( const ClientDesc & inst );
    const ClientDesc & operator=( const ClientDesc & inst );

    int id_;
    String login_;
    String password_;
    String firstName_;
    String lastName_;
    String suffix_;
};

class Environment: public LogicComponent
{
    URHO3D_OBJECT( Environment, LogicComponent )
public:
    Environment( Context * context );
    ~Environment();

    static void RegisterComponent( Context * context );

    bool IsClient() const;
    bool IsServer() const;

    virtual void Start() override;
    virtual void DelayedStart() override;
    void Stop() override;
    void Update( float timeStep ) override;

    /// Network commands.
    void StartServer( int port=-1 );
    void Connect( const ClientDesc & desc, const String & addr=String(), int port=-1 );
    void Disconnect();

    bool SendChatMessage( const String & message );
    void RequestItemSelect( Node * node );

    /// Callbacks from event handlers.
    virtual bool ClientConnected( int id, const VariantMap & identity, String & errMsg );
    virtual void ClientDisconnected( int id );
    virtual void ConnectedToServer( bool success );
    virtual void StartedServer( bool success );
    virtual void ConnectionResult( const String & errMsg );
    virtual void ChatMessage( const String & user, const String & message );
    virtual void SelectRequest( const ClientDesc & c, RefFrame * rf );
    virtual void HandleConsoleCommand( const String & cmd, const String & id=String() );

protected:
    /// Subscribe to update, UI and network events.
    virtual void SubscribeToEvents();

    /// Handle pressing the start server button.
    void HandleStartServer(StringHash eventType, VariantMap& eventData);
    /// Handle connection status change (just update the buttons that should be shown.)
    void HandleConnectionStatus(StringHash eventType, VariantMap& eventData);
    /// Handle a client connecting to the server.
    void HandleClientConnected(StringHash eventType, VariantMap& eventData);
    /// Handle a client disconnecting from the server.
    void HandleClientDisconnected(StringHash eventType, VariantMap& eventData);
    /// Handle remote event from server which tells our controlled object node ID.
    void HandleAssignClientId(StringHash eventType, VariantMap& eventData);

    /// Handle connection status result.
    void HandleConnectionResult( StringHash eventType, VariantMap & eventData );
    /// Handle chat messages both on client and server.
    void HandleChatMessage( StringHash eventType, VariantMap & eventData );
    /// Handle select request.
    void HandleSelectRequest( StringHash eventType, VariantMap & eventData );


    void HandleKeyDown( StringHash eventType, VariantMap & eventData );
    void HandleConsoleCommand( StringHash eventType, VariantMap & eventData );

private:
    void SetupConsole();
    void IncrementTime( float secs_dt );
    void UpdateDynamicNodes( Float  secs_dt );
    void UpdateEvolvingNodes( Timestamp ticks_dt );
    int  UniqueId();

    /// Global time
    Timestamp T_;
    /// Delta times in seconds and in ticks.
    Float     secsDt_;
    Timestamp ticksDt_;


    /// In the case of client assign client Id;
    int clientId_;

    /// Network state holders
    bool startingServer_;
    bool connectingToServer_;

    /// Client/Server functionality
    HashMap<Connection*, ClientDesc> connections_;
};

}


#endif


