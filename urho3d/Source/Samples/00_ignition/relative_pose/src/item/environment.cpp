
#include "environment.h"
#include "physics_frame.h"
#include "settings.h"

namespace Ign
{

// UDP port we will use
static const unsigned short SERVER_PORT = 21345;
// Identifier for our custom remote event we use to tell the client which object they control
static const StringHash E_IGN_CLIENTID("ClientID");
static const StringHash E_IGN_CONNECTIONRESULT("ConnectionResult");
static const StringHash E_IGN_CHATMESSAGE("ChatMessage");
// Identifier for the node ID parameter in the event data
static const StringHash P_ID("ID");
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
    clientId_ = -1;

    startingServer_     = false;
    connectingToServer_ = false;
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

            while ( maxTicksDt > 0 )
            {
                const Timestamp dt = ( ticksDt > maxTicksDt ) ? maxTicksDt : ticksDt;
                UpdateEvolvingNodes( dt );
                ticksDt -= dt;
            }
        }
    }
}

void Environment::StartServer( int port )
{
    Network * n = GetSubsystem<Network>();
    n->StartServer( port );
    startingServer_ = true;
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
    clientId_ = -1;

    Scene * s = GetScene();
    VariantMap identity;
    identity[P_LOGIN]    = desc.login_;
    identity[P_PASSWORD] = desc.password_;

    network->Connect( address, p, s );

    connectingToServer_ = true;
}

void Environment::Disconnect()
{
    clientId_ = -1;

    Network * network = GetSubsystem<Network>();
    Connection * serverConnection = network->GetServerConnection(); 
    if ( serverConnection )
    {
        serverConnection->Disconnect();
        Scene * s = GetScene();
        s->Clear( true, false );
        clientId_ = -1;
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

bool Environment::ClientConnected( int id, const VariantMap & identity, String & errMsg )
{
    return true;
}

void Environment::ClientDisconnected( int id )
{
    URHO3D_LOGINFOF( "Client disconnected: %i", id );
}

void Environment::ConnectedToServer( bool success )
{
    URHO3D_LOGINFOF( "Connected to server: %b", success );
}

void Environment::StartedServer( bool success )
{
    URHO3D_LOGINFOF( "Started server: %b", success );
}

void Environment::ConnectionResult( const String & errMsg )
{
    URHO3D_LOGINFOF( "Connection refused: %s", errMsg.CString() );
}

void Environment::ChatMessage( const String & user, const String & message )
{
    URHO3D_LOGINFOF( "Chat message: %s: %s", user.CString(), message.CString() );
}

void Environment::SubscribeToEvents()
{
    // Subscribe to network events
    SubscribeToEvent( E_SERVERCONNECTED,    URHO3D_HANDLER(Environment, HandleConnectionStatus));
    SubscribeToEvent( E_SERVERDISCONNECTED, URHO3D_HANDLER(Environment, HandleConnectionStatus));
    SubscribeToEvent( E_CONNECTFAILED,      URHO3D_HANDLER(Environment, HandleConnectionStatus));
    SubscribeToEvent( E_CLIENTCONNECTED,    URHO3D_HANDLER(Environment, HandleClientConnected));
    SubscribeToEvent( E_CLIENTDISCONNECTED, URHO3D_HANDLER(Environment, HandleClientDisconnected));
    // This is a custom event, sent from the server to the client. It tells the 
    // node ID of the object the client should control
    SubscribeToEvent(E_IGN_CLIENTID, URHO3D_HANDLER( Environment, HandleAssignClientId) );
    SubscribeToEvent(E_IGN_CONNECTIONRESULT, URHO3D_HANDLER( Environment, HandleConnectionResult) );
    SubscribeToEvent(E_IGN_CHATMESSAGE, URHO3D_HANDLER( Environment, HandleChatMessage) );
    // Events sent between client & server (remote events) must be explicitly registered 
    // or else they are not allowed to be received
    GetSubsystem<Network>()->RegisterRemoteEvent( E_IGN_CLIENTID );
    GetSubsystem<Network>()->RegisterRemoteEvent( E_IGN_CONNECTIONRESULT );
    GetSubsystem<Network>()->RegisterRemoteEvent( E_IGN_CHATMESSAGE );
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

    URHO3D_LOGINFOF( "############################\n"
                     "    Connection status:\n"
                     "    Server running: %b\n"
                     "    Connected:      %b\n"
                     "############################" );
}

void Environment::HandleClientConnected( StringHash eventType, VariantMap & eventData )
{
    using namespace ClientConnected;                                                 
                                                                                 
    // When a client connects, assign to scene to begin scene replication            
    Connection * newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    const int id = UniqueId();
    const VariantMap & identity = newConnection->GetIdentity();

    // First of all check if it contains needed data.
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
    newConnection->SetScene( s );

    // Save the connection and assign it unique id.
    connections_[newConnection] = d;

    // Send client its assigned Id.
    VariantMap args;
    args[P_ID] = id;
    newConnection->SendRemoteEvent( E_IGN_CLIENTID, true, args ); 

    {
        URHO3D_LOGINFOF( "New client connected, id assigned: %i", id );
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
        URHO3D_LOGINFOF( "Client %i disconnected", id );
    }

    ClientDisconnected( id );
}

void Environment::HandleAssignClientId( StringHash eventType, VariantMap & eventData )
{
    clientId_ = eventData[P_ID].GetUInt();
    URHO3D_LOGINFOF( "Client id assigned: %i", clientId_ );
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
    }
}

int Environment::UniqueId()
{
    int newId = 0;
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




}



