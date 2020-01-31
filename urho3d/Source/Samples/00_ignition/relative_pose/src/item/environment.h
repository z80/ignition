
#ifndef __ENVIRONMENT_H_
#define __ENVIRONMENT_H_

#include "Urho3D/Urho3DAll.h"
#include "data_types.h"

using namespace Urho3D;

namespace Ign
{

class RefFrame;
class CameraFrame;

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
    int    cameraFrameId_; // CameraId_ ID to find it easier.
};

class Environment: public LogicComponent
{
    URHO3D_OBJECT( Environment, LogicComponent )
public:
    Environment( Context * context );
    virtual ~Environment();

    static Environment * environment( Context * context );

    static void RegisterComponent( Context * context );

    bool IsClient() const;
    bool IsServer() const;

    virtual void Start() override;
    virtual void DelayedStart() override;
    void Stop() override;
    void Update( float timeStep ) override;
    void PostUpdate( float timeStep ) override;
    //void FixedUpdate( float timeStep ) override; // Here controlls are appied.

    /// Network commands.
    void StartServer( int port=-1 );
    void Connect( const ClientDesc & desc, const String & addr=String(), int port=-1 );
    void Disconnect();

    bool SendChatMessage( const String & message );
    void SendRequestItemSelect( Node * node );
    void SendRequestCenter( RefFrame * rf );
    /// This one is virtual as client may transfer additional information
    /// besides of the event itself.
    virtual void SendRequestTrigger( RefFrame * rf, VariantMap & data );

    ClientDesc & clientDesc();

    /// Callbacks from event handlers.
    virtual bool ClientConnected( int id, const VariantMap & identity, String & errMsg );
    virtual void ClientDisconnected( int id );
    virtual void ConnectedToServer( bool success );
    virtual void StartedServer( bool success );
    virtual void ConnectionResult( const String & errMsg );
    virtual void ChatMessage( const String & user, const String & message );
    virtual void SelectRequest( const ClientDesc & c, RefFrame * rf );
    virtual void CenterRequest( const ClientDesc & c, RefFrame * rf );
    virtual void TriggerRequest( const ClientDesc & c, RefFrame * rf, const VariantMap & data );
    virtual void ConsoleCommand( const String & cmd, const String & id=String() );

protected:
    /// Subscribe to update, UI and network events.
    virtual void SubscribeToEvents();
    virtual void CreateReplicatedContentServer();
    virtual void CreateReplicatedContentClient( CameraFrame * camera );

    /// Handle connection status change (just update the buttons that should be shown.)
    void HandleConnectionStatus(StringHash eventType, VariantMap& eventData);
    /// Handle a client connecting to the server.
    void HandleClientConnected(StringHash eventType, VariantMap& eventData);
    /// Handle client identity.
    void HandleClientIdentity(StringHash eventType, VariantMap& eventData);
    /// Handle a client disconnecting from the server.
    void HandleClientDisconnected(StringHash eventType, VariantMap& eventData);
    /// Handle remote event from server which tells our controlled object node ID.
    void HandleAssignClientId_Remote(StringHash eventType, VariantMap& eventData);

    /// Handle connection status result.
    void HandleConnectionResult_Remote( StringHash eventType, VariantMap & eventData );
    /// Handle chat messages both on client and server.
    void HandleChatMessage_Remote( StringHash eventType, VariantMap & eventData );
    /// Handle select request.
    void HandleSelectRequest_Remote( StringHash eventType, VariantMap & eventData );
    /// Handler center request. Camera should be parented to to object selected.
    void HandleCenterRequest_Remote( StringHash eventType, VariantMap & eventData );
    /// Trigger request.
    void HandleTriggerRequest_Remote( StringHash eventType, VariantMap & eventData );


    void HandleKeyDown( StringHash eventType, VariantMap & eventData );
    void HandleMouseDown( StringHash eventType, VariantMap & eventData );
    void HandleMouseUp( StringHash eventType, VariantMap & eventData );
    void HandleMouseWheel( StringHash eventType, VariantMap & eventData );
    void HandleConsoleCommand( StringHash eventType, VariantMap & eventData );
    void HandleSelectRequest( StringHash eventType, VariantMap & eventData );
    void HandleCenterRequest( StringHash eventType, VariantMap & eventData );
    void HandleTriggerRequest( StringHash eventType, VariantMap & eventData );

private:
    void SetupConsole();
    void IncrementTime( float secs_dt );
    void UpdateDynamicNodes( Float  secs_dt );
    void UpdateEvolvingNodes( Timestamp ticks_dt );
    void UpdateDynamicGeometryNodes( bool isServer, bool isClient );
    void CaptureControls();
    void ApplyControls();
public:
    CameraFrame * FindCameraFrame( const ClientDesc & cd );
    CameraFrame * FindCameraFrame();
private:
    RefFrame    * FindSelectedFrame( const ClientDesc & cd );
    RefFrame    * FindSelectedFrame();
    void ProcessLocalVisuals();
    int  UniqueId();
    void LoadTranslations();

    /// Global time
    Timestamp T_;
    /// Delta times in seconds and in ticks.
    Float     secsDt_;
    Timestamp ticksDt_;


    /// In the case of client assign client Id;
    ClientDesc clientDesc_;
    Controls   controls_;

    /// Network state holders
    bool startingServer_;
    bool connectingToServer_;

    /// Client/Server functionality
    HashMap<Connection*, ClientDesc> connections_;
    HashMap<int, Connection *>       clientIds_;

    /// Draw debug geometry.
    bool drawDebugGeometry_;
};

}


#endif


