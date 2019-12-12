
#include "environment.h"
#include "physics_frame.h"
#include "settings.h"

namespace Ign
{

// UDP port we will use
static const unsigned short SERVER_PORT = 21345;
// Identifier for our custom remote event we use to tell the client which object they control
static const StringHash E_CLIENTOBJECTID("ClientID");
// Identifier for the node ID parameter in the event data
static const StringHash P_ID("ID");


Environment::Environment( Context * context )
    : LogicComponent( context )
{
    T_        = 0;
    secsDt_   = 0.0;
    ticksDt_  = 0;
    clientId_ = -1;
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
        MarkNetworkUpdate();
        rk * n = GetSubsystem<Network>();

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
}

void Environment::Connect( const String & addr, int port )
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
    network->Connect( address, p );
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
    SubscribeToEvent(E_CLIENTID, URHO3D_HANDLER( Environment, HandleClientID) );
    // Events sent between client & server (remote events) must be explicitly registered 
    // or else they are not allowed to be received
    GetSubsystem<Network>()->RegisterRemoteEvent( E_CLIENTID );
}

void Environment::HandleStartServer( StringHash eventType, VariantMap & eventData )
{
}

void Environment::HandleConnectionStatus( StringHash eventType, VariantMap & eventData )
{
}

void Environment::HandleClientConnected( StringHash eventType, VariantMap & eventData )
{
    using namespace ClientConnected;                                                 
                                                                                 
    // When a client connects, assign to scene to begin scene replication            
    Connection * newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    Scene * s = GetScene();
    newConnection->SetScene( s );


    // Send client its assigned Id.
    VariantMap args;
    args[P_ID] = id;
    newConnection->SendremoteEvent( E_CLIENTID, true, args ); 

}

void Environment::HandleClientDisconnected( StringHash eventType, VariantMap & eventData )
{
}

void Environment::HandleClientObjectID( StringHash eventType, VariantMap & eventData )
{
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
        HashMap<int, Connection *>::Iterator i = connections_.Find( newId );
        if ( i == connections_.End() )
            break;
        newId += 1;
    }
    return newId;
}




}



