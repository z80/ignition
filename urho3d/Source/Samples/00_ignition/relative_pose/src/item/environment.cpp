
#include "environment.h"
#include "physics_frame.h"
#include "settings.h"

namespace Ign
{

Environment::Environment( Context * context )
    : LogicComponent( context )
{
    T_        = 0;
    secsDt_   = 0.0;
    ticksDt_  = 0;

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



}



