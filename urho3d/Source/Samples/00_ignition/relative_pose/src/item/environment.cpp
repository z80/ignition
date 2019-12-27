
#include "environment.h"
#include "ref_frame.h"
#include "physics_frame.h"
#include "evolving_frame.h"
#include "camera_frame.h"
#include "settings.h"

#include "Notifications.h"
#include "ConfigManager.h"
#include "ControllerInput.h"

namespace Ign
{

// UDP port we will use
static const unsigned short SERVER_PORT = 21345;
// Identifier for our custom remote event we use to tell the client which object they control
static const StringHash E_IGN_CLIENTID("ClientID");
static const StringHash E_IGN_CONNECTIONRESULT("ConnectionResult");
static const StringHash E_IGN_CHATMESSAGE("ChatMessage");
static const StringHash E_IGN_SELECTREQUEST("SelectRequest");
// Identifier for the node ID parameter in the event data
static const StringHash P_ID("Id");
static const StringHash P_CAMERAID("CameraId");
// Login
static const StringHash P_LOGIN("LOGIN");
static const StringHash P_PASSWORD("PASSWORD");
static const StringHash P_FIRSTNAME("FIRST_NAME");
static const StringHash P_LASTNAME("LAST_NAME");
static const StringHash P_SUFFIX("SUFFIX");
static const StringHash P_PREFIX("PREFIX");
// Chat messages
static const StringHash P_CHATNAME("CHATNAME");
static const StringHash P_CHATTEXT("CHATTEXT");



ClientDesc::ClientDesc()
{
    id_ = -1;
}

ClientDesc::ClientDesc( const ClientDesc & inst )
{
    *this = inst;
}

const ClientDesc & ClientDesc::operator=( const ClientDesc & inst )
{
    if ( this != &inst )
    {
        id_        = inst.id_;
        login_     = inst.login_;
        password_  = inst.password_;
        firstName_ = inst.firstName_;
        lastName_  = inst.lastName_;
        suffix_    = inst.suffix_;
    }

    return *this;
}








Environment::Environment( Context * context )
    : LogicComponent( context )
{
    T_        = 0;
    secsDt_   = 0.0;
    ticksDt_  = 0;
    clientDesc_.id_ = -1;

    startingServer_     = false;
    connectingToServer_ = false;

    Variant a;
    a = this;
    SetGlobalVar( StringHash( "Environment" ), a );
}

Environment::~Environment()
{

}

void Environment::RegisterComponent( Context * context )
{
    context->RegisterFactory<Environment>();
}

bool Environment::IsClient() const
{
    Network * network = GetSubsystem<Network>();
    Connection* serverConnection = network->GetServerConnection();
    const bool client = (serverConnection != nullptr);
    return client;
}

bool Environment::IsServer() const
{
    Network * network = GetSubsystem<Network>();
    const bool serverRunning = network->IsServerRunning();
    return serverRunning;
}

void Environment::Start()
{
    LoadTranslations();
    SetupConsole();
    SubscribeToEvents();
    SetUpdateEventMask( USE_UPDATE );
}

void Environment::DelayedStart()
{

}

void Environment::Stop()
{

}

void Environment::Update( float timeStep )
{
    const bool isServer = IsServer();
    if ( isServer )
    {
        IncrementTime( timeStep );

        {
            Float secsDt = secsDt_;
            const Float maxSecsDt = Settings::maxDynamicsTimeStep();
            while ( secsDt > 0.0001 )
            {
                const Float dt = ( secsDt >= maxSecsDt ) ? maxSecsDt : secsDt;
                UpdateDynamicNodes( secsDt );
                secsDt -= dt;
            }
        }

        {
            Timestamp ticksDt = ticksDt_;
            const Timestamp maxTicksDt = Settings::maxEvolutionTimeStep();

            while ( ticksDt > 0 )
            {
                const Timestamp dt = ( ticksDt > maxTicksDt ) ? maxTicksDt : ticksDt;
                UpdateEvolvingNodes( dt );
                ticksDt -= dt;
            }
        }
    }

    // This runs locally.
    // Update all RefFrame objects based on current user id.
    {
        Float secsDt = secsDt_;
        const Float maxSecsDt = Settings::maxDynamicsTimeStep();
        while ( secsDt > 0.0001 )
        {
            const Float dt = ( secsDt >= maxSecsDt ) ? maxSecsDt : secsDt;
            ProcessLocalVisuals( secsDt );
            secsDt -= dt;
        }
    }
}

//void Environment::FixedUpdate( float timeStep )
//{

//}

void Environment::StartServer( int port )
{
    Network * n = GetSubsystem<Network>();
    const int p = (port > 0) ? port : SERVER_PORT;
    n->StartServer( p );
    startingServer_ = true;
    clientDesc_.id_ = 0;

    // Create camera frame for newly connected client.
    Scene * s = GetScene();
    CameraFrame * cf = s->CreateComponent<CameraFrame>( REPLICATED );
    clientDesc_.cameraFrameId_ = cf->GetID();
    cf->userId_ = clientDesc_.id_;
    cf->SetCreatedBy( clientDesc_.id_ );

    CreateReplicatedContentServer();
    CreateReplicatedContentClient( cf );
}

void Environment::Connect( const ClientDesc & desc, const String & addr, int port )
{
    Network * network = GetSubsystem<Network>();
    String address;
    if ( addr.Empty() )
        address = "localhost";
    else
        address = addr;
    int p;
    if ( port > 0 )
        p = port;
    else
        p = SERVER_PORT;
    clientDesc_.id_ = -1;

    Scene * s = GetScene();
    VariantMap identity;
    identity[P_LOGIN]    = desc.login_;
    identity[P_PASSWORD] = desc.password_;

    {
        const unsigned qty = identity.Size();
        {
            //URHO3D_LOGINFOF( "Identity entries qty: %i", qty );
            const String stri = "Identity entries qty: " + String( qty );
            Notifications::AddNotification( GetContext(), stri );
        }
        for ( VariantMap::ConstIterator it=identity.Begin();
              it!=identity.End(); it++ )
        {
            const StringHash sh = it->first_;
            const String id = sh.ToString();
            const String stri = it->second_.GetString();
            const String v = "id: " + id + ", value: " + stri;
            //URHO3D_LOGINFO( v );
            Notifications::AddNotification( GetContext(), v );
        }
    }

    network->Connect( address, p, s, identity );

    connectingToServer_ = true;
}

void Environment::Disconnect()
{
    clientDesc_.id_ = -1;

    Network * network = GetSubsystem<Network>();
    Connection * serverConnection = network->GetServerConnection(); 
    if ( serverConnection )
    {
        serverConnection->Disconnect();
        Scene * s = GetScene();
        s->Clear( true, false );
        clientDesc_.id_ = -1;
    }
    else if ( network->IsServerRunning() )
    {
        network->StopServer();
    }
}

bool Environment::SendChatMessage( const String & message )
{
    Network * n = GetSubsystem<Network>();
    Connection * c = n->GetServerConnection();
    if ( !c )
    {
        if ( !n->IsServerRunning() )
            // If neither client nor server return failure
            return false;

        // We are server.
        VariantMap data;
        data[P_CHATNAME] = "Server";
        data[P_CHATTEXT] = message;
        n->BroadcastRemoteEvent( E_IGN_CHATMESSAGE, true, data );
    }
    else
    {
        // We are client.
        VariantMap data;
        data[P_CHATTEXT] = message;
        c->SendRemoteEvent( E_IGN_CHATMESSAGE, true, data );
    }

    // Return success
    return true;
}

void Environment::RequestItemSelect( Node * node )
{
    RefFrame * rf = RefFrame::refFrame( node->GetScene() );
    if ( !rf )
    {
        //URHO3D_LOGINFO( "Selecting something not having RefFrame" );
        return;
    }

    const unsigned id = rf->GetID();

    Network * n = GetSubsystem<Network>();
    const bool serverRunning = n->IsServerRunning();
    Connection * c = n->GetServerConnection();

    if ( (!serverRunning) && ( !c ) )
    {
        //URHO3D_LOGINFO( "Need to be in game to select something!" );
        Notifications::AddNotification( GetContext(), "Need to be in game to select something!" );
        return;
    }

    if ( serverRunning )
    {
        // Call callback locally.
        ClientDesc c;
        c.login_ = "_SERVER_";
        SelectRequest( c, rf );
    }
    else
    {
        VariantMap data;

        data[P_ID] = id;
        c->SendRemoteEvent( E_IGN_SELECTREQUEST, true, data );
    }
}

ClientDesc & Environment::clientDesc()
{
    return clientDesc_;
}

bool Environment::ClientConnected( int id, const VariantMap & identity, String & errMsg )
{
    return true;
}

void Environment::ClientDisconnected( int id )
{
    //URHO3D_LOGINFOF( "Client disconnected: %i", id );
    const String stri = "Client disconnected: " + String( id );
    Notifications::AddNotification( GetContext(), stri );
}

void Environment::ConnectedToServer( bool success )
{
    //URHO3D_LOGINFOF( "Connected to server: %b", success );
    const String stri = "Connected to server: " + String( success );
    Notifications::AddNotification( GetContext(), stri );
}

void Environment::StartedServer( bool success )
{
    //URHO3D_LOGINFOF( "Started server: %b", success );
    const String stri = "Started server: " + String( success );
    Notifications::AddNotification( GetContext(), stri );
}

void Environment::ConnectionResult( const String & errMsg )
{
    //URHO3D_LOGINFOF( "Connection refused: %s", errMsg.CString() );
    const String stri = "Connection refused: " + errMsg;
    Notifications::AddNotification( GetContext(), stri );
}

void Environment::ChatMessage( const String & user, const String & message )
{
    //URHO3D_LOGINFOF( "Chat message: %s: %s", user.CString(), message.CString() );
    const String stri = "Char message: " + user + " " + message;
    Notifications::AddNotification( GetContext(), stri );
}

void Environment::SelectRequest( const ClientDesc & c, RefFrame * rf )
{
    //URHO3D_LOGINFOF( "User %s wants to select: %s", c.login_.CString(), rf->name().CString() );
    const String stri = "User " + c.login_ + " wants to select " + rf->name();
    Notifications::AddNotification( GetContext(), stri );
}

void Environment::ConsoleCommand( const String & cmd, const String & id )
{
    const String c = cmd.ToLower().Trimmed();
    if ( (c == "exit") || (c == "quit") )
    {
        Engine * e = GetSubsystem<Engine>();
        e->Exit();
    }
    //else if ( cmd == "connect" )
}

void Environment::SubscribeToEvents()
{
    // Subscribe to network events
    SubscribeToEvent( E_SERVERCONNECTED,    URHO3D_HANDLER(Environment, HandleConnectionStatus));
    SubscribeToEvent( E_SERVERDISCONNECTED, URHO3D_HANDLER(Environment, HandleConnectionStatus));
    SubscribeToEvent( E_CONNECTFAILED,      URHO3D_HANDLER(Environment, HandleConnectionStatus));
    SubscribeToEvent( E_CLIENTCONNECTED,    URHO3D_HANDLER(Environment, HandleClientConnected));
    SubscribeToEvent( E_CLIENTIDENTITY,     URHO3D_HANDLER(Environment, HandleClientIdentity));
    SubscribeToEvent( E_CLIENTDISCONNECTED, URHO3D_HANDLER(Environment, HandleClientDisconnected));
    // This is a custom event, sent from the server to the client. It tells the 
    // node ID of the object the client should control
    SubscribeToEvent(E_IGN_CLIENTID, URHO3D_HANDLER( Environment, HandleAssignClientId) );
    SubscribeToEvent(E_IGN_CONNECTIONRESULT, URHO3D_HANDLER( Environment, HandleConnectionResult) );
    SubscribeToEvent(E_IGN_CHATMESSAGE, URHO3D_HANDLER( Environment, HandleChatMessage) );
    SubscribeToEvent(E_IGN_SELECTREQUEST, URHO3D_HANDLER( Environment, HandleSelectRequest) );

    // Events sent between client & server (remote events) must be explicitly registered 
    // or else they are not allowed to be received
    GetSubsystem<Network>()->RegisterRemoteEvent( E_IGN_CLIENTID );
    GetSubsystem<Network>()->RegisterRemoteEvent( E_IGN_CONNECTIONRESULT );
    GetSubsystem<Network>()->RegisterRemoteEvent( E_IGN_CHATMESSAGE );
    GetSubsystem<Network>()->RegisterRemoteEvent( E_IGN_SELECTREQUEST );


    // Key events.
    SubscribeToEvent( E_KEYDOWN, URHO3D_HANDLER( Environment, HandleKeyDown ) );
    // Mouse buttons and wheel.
    SubscribeToEvent( E_MOUSEBUTTONDOWN, URHO3D_HANDLER( Environment, HandleMouseDown ) );
    SubscribeToEvent( E_MOUSEBUTTONUP,   URHO3D_HANDLER( Environment, HandleMouseUp ) );
    SubscribeToEvent( E_MOUSEWHEEL,      URHO3D_HANDLER( Environment, HandleMouseWheel ) );
    // Console commands.
    SubscribeToEvent( E_CONSOLECOMMAND, URHO3D_HANDLER( Environment, HandleConsoleCommand ) );
}

void Environment::CreateReplicatedContentServer()
{

}

void Environment::CreateReplicatedContentClient( CameraFrame * camera )
{

}

void Environment::HandleConnectionStatus( StringHash eventType, VariantMap & eventData )
{
    Network * network = GetSubsystem<Network>();
    Connection * serverConnection = network->GetServerConnection();
    const bool serverRunning = network->IsServerRunning();

    // Show and hide buttons so that eg. Connect and Disconnect are never shown at the same time
    //connectButton_->SetVisible(!serverConnection && !serverRunning);
    //disconnectButton_->SetVisible(serverConnection || serverRunning);
    //startServerButton_->SetVisible(!serverConnection && !serverRunning);
    //textEdit_->SetVisible(!serverConnection && !serverRunning);
    
    if ( startingServer_ )
    {
        startingServer_ = false;
        StartedServer( serverRunning );
    }
    else if ( connectingToServer_ )
    {
        connectingToServer_ = false;
        ConnectedToServer( serverConnection != nullptr );
    }


    const bool canConnect     = !serverConnection && !serverRunning;
    const bool canDisconnect  = serverConnection || serverRunning;
    const bool canStartServer = !serverConnection && !serverRunning;

    const String stri =
                    String( "\n############################\n" ) +
                    String( "    Connection status:\n" ) +
                    String( "    Server running: " ) + String( serverRunning ? "true" : "false" ) + String( "\n" ) +
                     "    Connected:      " + String( (serverConnection) ? "true" : "false" ) + String( "\n" ) +
                     "############################";
    Notifications::AddNotification( GetContext(), stri );
}

void Environment::HandleClientConnected( StringHash eventType, VariantMap & eventData )
{
    {
        //URHO3D_LOGINFO( "New client connected" );
        const String stri = "New client connected";
        Notifications::AddNotification( GetContext(), stri );
    }
}

void Environment::HandleClientIdentity( StringHash eventType, VariantMap & eventData )
{
    using namespace ClientIdentity;

    Connection * newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    const int id = UniqueId();
    const VariantMap & identity = newConnection->GetIdentity();

    // First of all check if it contains needed data.
    {
        const unsigned qty = identity.Size();
        {
            URHO3D_LOGINFOF( "Identity entries qty: %i", qty );
            const String stri = "Identity entries qty: " + String( qty );
            Notifications::AddNotification( GetContext(), stri );
        }
        for ( VariantMap::ConstIterator it=identity.Begin();
              it!=identity.End(); it++ )
        {
            const StringHash sh = it->first_;
            const String id = sh.ToString();
            const String stri = it->second_.GetString();
            const String v = "id: " + id + ", value: " + stri;
            {
                //URHO3D_LOGINFO( v );
                Notifications::AddNotification( GetContext(), v );
            }
        }
    }


    ClientDesc d;
    VariantMap::ConstIterator it = identity.Find( P_LOGIN );
    bool hasLogin    = (it != identity.End());
    if ( hasLogin )
        d.login_ = it->second_.GetString();
    it = identity.Find( P_PASSWORD );
    bool hasPassword = (it != identity.End());
    if ( hasPassword )
        d.password_ = it->second_.GetString();

    bool validClient = hasLogin && hasPassword;

    String errMsg;
    if ( validClient )
        validClient = ClientConnected( id, identity, errMsg );
    else
        errMsg = "Client information provided doesn\'t contain either \'login\', \'password\' or both";

    if ( !validClient )
    {
        VariantMap data;
        data[P_CHATTEXT] = errMsg;
        newConnection->SendRemoteEvent( E_IGN_CONNECTIONRESULT, true, data );
        newConnection->SendRemoteEvents();
        newConnection->Disconnect();
        return;
    }


    Scene * s = GetScene();

    // Create camera frame for newly connected client.
    CameraFrame * cf = s->CreateComponent<CameraFrame>( REPLICATED );
    d.cameraFrameId_ = cf->GetID();
    cf->userId_ = id;
    cf->SetCreatedBy( clientDesc_.id_ );

    CreateReplicatedContentClient( cf );

    // When a client connects, assign to scene to begin scene replication
    newConnection->SetScene( s );

    // Save the connection and assign it unique id.
    connections_[newConnection] = d;

    // Send client its assigned Id.
    VariantMap args;
    args[P_ID]       = id;
    args[P_CAMERAID] = cf->GetID();
    newConnection->SendRemoteEvent( E_IGN_CLIENTID, true, args );

    {
        //URHO3D_LOGINFOF( "New client connected, id assigned: %i", id );
        const String stri = "New client connected, id assigned: " + String( id );
        Notifications::AddNotification( GetContext(), stri );
    }
}

void Environment::HandleClientDisconnected( StringHash eventType, VariantMap & eventData )
{
    using namespace ClientConnected;

    Connection * connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    HashMap<Connection *, ClientDesc>::Iterator i = connections_.Find( connection );
    if ( i == connections_.End() )
        return;

    connections_.Erase( connection );
    const int id = i->second_.id_;
    {
        //URHO3D_LOGINFOF( "Client %i disconnected", id );
        const String stri = "Client " + String(id) + " disconnected";
        Notifications::AddNotification( GetContext(), stri );
    }

    ClientDisconnected( id );
}

void Environment::HandleAssignClientId( StringHash eventType, VariantMap & eventData )
{
    clientDesc_.id_            = eventData[P_ID].GetUInt();
    clientDesc_.cameraFrameId_ = eventData[P_CAMERAID].GetUInt();
    //URHO3D_LOGINFOF( "Client id assigned: %i", clientDesc_.id_ );
    {
        const String stri = "Client id assigned: " + String( clientDesc_.id_ );
        Notifications::AddNotification( GetContext(), stri );
    }
    {
        const String stri = "Camera frame id assigned: " + String( clientDesc_.cameraFrameId_ );
        Notifications::AddNotification( GetContext(), stri );
    }
}

void Environment::HandleConnectionResult( StringHash eventType, VariantMap & eventData )
{
    const String & errMsg = eventData[P_CHATTEXT].GetString();
    ConnectionResult( errMsg );
}

void Environment::HandleChatMessage( StringHash eventType, VariantMap & eventData )
{
    const bool isServer = IsServer();
    VariantMap data;
    if ( isServer )
    {
        Network * n = GetSubsystem<Network>();

        using namespace RemoteEventData;
        Connection* sender = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
        HashMap<Connection*, ClientDesc>::Iterator it = connections_.Find( sender );
        if ( it == connections_.End() )
            return;

        const ClientDesc & c = it->second_;

        data[P_CHATNAME] = c.firstName_ + " " + c.lastName_ + " (" + c.login_ + ")";
        data[P_CHATTEXT] = eventData[P_CHATTEXT];

        n->BroadcastRemoteEvent( E_IGN_CHATMESSAGE, true, data );
    }
    else
        data = eventData;

    const String user = data[P_CHATNAME].GetString();
    const String message = data[P_CHATTEXT].GetString();
    ChatMessage( user, message );
}

void Environment::HandleSelectRequest( StringHash eventType, VariantMap & eventData )
{
    const unsigned id = eventData[P_ID].GetUInt();

    using namespace RemoteEventData;
    Connection* sender = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    HashMap<Connection*, ClientDesc>::Iterator it = connections_.Find( sender );
    if ( it == connections_.End() )
        return;

    const ClientDesc & c = it->second_;

    Scene * s = GetScene();
    RefFrame * rf = RefFrame::refFrame( s, id );

    SelectRequest( c, rf );
}

void Environment::HandleKeyDown( StringHash eventType, VariantMap & eventData )
{
    const int key = eventData[KeyDown::P_KEY].GetInt();
    if ( key == SDL_SCANCODE_KP_8 )
    {
        Console * c = GetSubsystem<Console>();
        if (!c)
            return;
        const bool isVisible = c->IsVisible();
        c->SetVisible( !isVisible );
    }
    else if ( key == KEY_ESCAPE )
    {
        Console * c = GetSubsystem<Console>();
        if (!c)
            return;
        //c->SetVisible( false );
    }
}

void Environment::HandleMouseDown( StringHash eventType, VariantMap & eventData )
{

}

void Environment::HandleMouseUp( StringHash eventType, VariantMap & eventData )
{

}

void Environment::HandleMouseWheel( StringHash eventType, VariantMap & eventData )
{

}

void Environment::HandleConsoleCommand( StringHash eventType, VariantMap & eventData )
{
    using namespace ConsoleCommand;
    const String cmd = eventData[P_COMMAND].GetString();
    if ( eventData.Contains( P_ID ) )
    {
        const String id = eventData[P_ID].GetString();
        ConsoleCommand( cmd, id );
    }
    else
        ConsoleCommand( cmd );
}

void Environment::SetupConsole()
{
    auto * console = GetSubsystem<Console>();
    console->SetNumRows(GetSubsystem<Graphics>()->GetHeight() / 16);
    console->SetNumBufferedRows(2 * console->GetNumRows());
    console->SetCommandInterpreter(GetTypeName());
    console->SetVisible( false );
    console->GetCloseButton()->SetVisible( false );

    console->AddAutoComplete("quit");
    console->AddAutoComplete("help");
}

void Environment::IncrementTime( float secs_dt )
{
    const Timestamp ticks = Settings::ticks( secs_dt );
    // Here apply time scale.

    T_ += ticks;

    ticksDt_ = ticks;

    // Compute dt for physics update.
    secsDt_ = Settings::secs( ticks );
    if ( secsDt_ > Settings::absLimitDynamicsTimeStep() )
        secsDt_ = 0.0;
}

void Environment::UpdateDynamicNodes( Float secs_dt )
{
    Scene * s = GetScene();
    if ( !s )
        return;

    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        // Try cast to dynamics integration node.
        // And if converted make time step.
        SharedPtr<Component> c = comps[i];
        PhysicsFrame * pf = c->Cast<PhysicsFrame>();
        if ( !pf )
            continue;
        pf->physicsStep( secs_dt );
    }
}

void Environment::UpdateEvolvingNodes( Timestamp ticks_dt )
{
    Scene * s = GetScene();
    if ( !s )
        return;

    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        // Try cast to evolving node.
        // And if converted make time step.
        SharedPtr<Component> c = comps[i];
        EvolvingFrame * ef = c->Cast<EvolvingFrame>();
        if ( !ef )
            continue;
        ef->evolveStep( ticks_dt );
    }
}

void Environment::CaptureControls()
{
    ControllerInput * ci = GetSubsystem<ControllerInput>();
    controls_ = ci->GetControls();
    if ( IsClient() )
    {
        Network * n = GetSubsystem<Network>();
        Connection * serverConnection = n->GetServerConnection();
        serverConnection->SetControls( controls_ );
    }
}

void Environment::ProcessControls()
{
    if ( !IsServer() )
        return;

    // Compose id to Connection hash map.
    clientIds_.Clear();
    {
        for ( HashMap<Connection *, ClientDesc>::ConstIterator it=connections_.Begin();
              it!=connections_.End(); it++ )
        {
            Connection * c = it->first_;
            const int id   = it->second_.id_;
            clientIds_.Insert( Pair<int, Connection *>( id, c ) );
        }
    }

    // Process own controlsand all client controls.
    Scene * s = GetScene();
    if ( !s )
        return;

    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        if ( !c )
            continue;

        RefFrame * rf = c->Cast<RefFrame>();
        if ( !rf )
            continue;

        // Check all clients this rf is selected by.
        const bool isSelectable = rf->IsSelectable();
        if ( isSelectable )
        {
            const Vector<int> selectedBy = rf->SelectedBy();
            const unsigned selQty = selectedBy.Size();
            for ( unsigned j=0; j<selQty; j++ )
            {
                const int userId = selectedBy[j];
                const bool acceptsUserCtrls = rf->AcceptsControls( userId );
                if ( !acceptsUserCtrls )
                    continue;
                HashMap<int, Connection *>::ConstIterator it = clientIds_.Find( userId );
                if ( it != clientIds_.End() )
                {
                    Connection * conn = it->second_;
                    const Controls ctrls = conn->GetControls();
                    cf->ApplyControls( ctrls );
                }
            }
        }

        // If camera always apply controls.
        CameraFrame * cf = rf->Cast<CameraFrame>();
        if ( cf )
        {
            const int userId = cf->CreatedBy();
            if ( userId == 0 )
                cf->ApplyControls( controls_ );
            else
            {
                HashMap<int, Connection *>::ConstIterator it = clientIds_.Find( userId );
                if ( it != clientIds_.End() )
                {
                    Connection * conn = it->second_;
                    const Controls ctrls = conn->GetControls();
                    cf->ApplyControls( ctrls );
                }
            }
        }
    }
}

CameraFrame * Environment::FindCameraFrame()
{
    const int frameId = clientDesc_.cameraFrameId_;
    if ( frameId < 0 )
        return nullptr;

    Scene * s = GetScene();
    if ( !s )
        return nullptr;

    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        if ( !c )
            continue;
        CameraFrame * cf = c->Cast<CameraFrame>();
        if ( !cf )
            continue;
        if ( cf->userId_ == clientDesc_.id_ )
            return cf;
    }

    return nullptr;
}

void Environment::ProcessLocalVisuals( Float secs_dt )
{
    CameraFrame * cam = FindCameraFrame();
    if ( !cam )
        return;

    RefFrame * originParent = cam->UpdatePose( secs_dt );

    Scene * s = GetScene();
    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        if ( !c )
            continue;
        RefFrame * rf = c->Cast<RefFrame>();
        if ( !rf )
            continue;
        // Do not process camera frames.
        CameraFrame * cf = c->Cast<CameraFrame>();
        if ( cf )
            continue;

        rf->computeRefState( originParent );
    }
}

int Environment::UniqueId()
{
    // Start with 1.
    // 0 is reserved for server.
    int newId = 1;
    while ( true )
    {
        bool found = false;
        for ( HashMap<Connection *, ClientDesc>::Iterator i = connections_.Begin(); 
              i!=connections_.End(); i++ )
        {
            const int existingId = i->second_.id_;
            if ( existingId == newId )
            {
                found = true;
                break;
            }
        }
        if ( !found )
            break;
        newId += 1;
    }
    return newId;
}

void Environment::LoadTranslations()
{
    Vector<String> result;
    Localization * localization = GetSubsystem<Localization>();
    ResourceCache * cache = GetSubsystem<ResourceCache>();

    // Get all translation files in the Data/Translations folder
    FileSystem * fs = GetSubsystem<FileSystem>();
    fs->ScanDir( result, GetSubsystem<FileSystem>()->GetProgramDir() + String("Data/Ign/Translations"), String("*.json"), SCAN_FILES, true );

#ifdef __ANDROID__
    result.Push("EN.json");
    result.Push("LV.json");
#endif

    for (auto it = result.Begin(); it != result.End(); ++it) {
        String file = (*it);

        //String filepath = "Translations/" + file;
        String filepath = GetSubsystem<FileSystem>()->GetProgramDir() + String("Data/Ign/Translations/") + file;
        // Filename is handled as a language
        file.Replace(".json", "", false);

        auto jsonFile = cache->GetResource<JSONFile>(filepath);
        if (jsonFile)
        {
            // Load the actual file in the system
            localization->LoadSingleLanguageJSON(jsonFile->GetRoot(), file);
            URHO3D_LOGINFO("Loading translation file '" + filepath + "' to '" + file + "' language");
        } else {
            URHO3D_LOGERROR("Translation file '" + filepath + "' not found!");
        }
    }

    // Finally set the application language
    localization->SetLanguage( GetSubsystem<ConfigManager>()->GetString("engine", "Language", "EN" ) );
}




}



