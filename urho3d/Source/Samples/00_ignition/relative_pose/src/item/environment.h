
#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include "Urho3D/Urho3DAll.h"
#include "data_types.h"

using namespace Urho3D;

namespace Ign
{

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
    void Connect( const String & addr=String(), int port=-1 );
    void Disconnect();

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

private:
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

    /// Client/Server functionality
    HashMap<Connection*, int> connections_;
};

}


#endif


