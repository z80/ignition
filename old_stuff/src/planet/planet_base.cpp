
#include "planet_base.h"
#include "world_mover.h"
#include "physics_world_2.h"
#include "game_data.h"
#include "planet_system_loader.h"
#include "MyEvents.h"

namespace Osp
{

PlanetBase::PlanetBase( Context * ctx )
    : ItemBase( ctx )
{
    _type = TPlanet;
}

PlanetBase::~PlanetBase()
{

}

bool PlanetBase::canOrbit( const ItemBase * a ) const
{
    const bool res = forces->canOrbit( a );
    return res;
}

Float PlanetBase::GM() const
{
    return forces->GM_;
}

Vector3d PlanetBase::relR() const
{
    return mover->relR();
}

Quaterniond PlanetBase::relQ() const
{
    return rotator->relQ();
}

Vector3d PlanetBase::relV() const
{
    return mover->relV();
}

Vector3d PlanetBase::relW() const
{
    return rotator->relW();
}

void PlanetBase::Start()
{
    Node * n = GetNode();
    dynamicsNode = n->CreateChild( "DynamicsNode" );
    mover = n->CreateComponent<KeplerMover>();
    //mover = dynamicsNode->CreateComponent<KeplerMover>();
    rotator = dynamicsNode->CreateComponent<KeplerRotator>();

    // Now GameData creates all the planets itself and puts those into this array.
    /*Scene * s = GetScene();
    Component * c = s->GetComponent( StringHash( "GameData" ), true );
    if ( c )
    {
        GameData * gd = c->Cast<GameData>();
        gd->planets.Push( SharedPtr<PlanetBase>( this ) );
    }*/

    forces = n->CreateComponent<PlanetForces>();

    subscribeToWorldEvents();
}

void PlanetBase::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    /*{
        const unsigned qty = surfaceBlocks.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<Block> b = surfaceBlocks[i];
            b->drawDebugGeometry( debug );
        }
    }*/
}

void PlanetBase::updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{

}

void PlanetBase::initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{

}

void PlanetBase::finitCollisions( PhysicsWorld2 * w2 )
{

}

bool PlanetBase::load( const JSONValue & v )
{
    {
        assert( v.Contains( "name" ) );
        const JSONValue & vv = v.Get( "name" );
        name = vv.GetString();
    }
    {
        assert( v.Contains( "kepler" ) );
        const JSONValue & vv = v.Get( "kepler" );
        const bool ok = PlanetLoader::loadKepler( vv, this );
        //mover->active = false;
    }
    {
        assert( v.Contains( "rotation" ) );
        const JSONValue & vv = v.Get( "rotation" );
        const bool ok = PlanetLoader::loadRotator( vv, this );
        //rotator->active = false;
    }
    {
        assert( v.Contains( "forces" ) );
        const JSONValue & vv = v.Get( "forces" );
        const bool res = forces->load( vv );
        return res;
    }
}




void PlanetBase::subscribeToWorldEvents()
{
    SubscribeToEvent( MyEvents::E_WORLD_PLANET_CHANGED, URHO3D_HANDLER( PlanetBase, OnWorldPlanetChanged ) );
    SubscribeToEvent( MyEvents::E_WORLD_ADJUSTED,       URHO3D_HANDLER( PlanetBase, OnWorldAdjusted ) );
}

void PlanetBase::OnWorldPlanetChanged( StringHash eventType, VariantMap & eventData )
{
    using namespace MyEvents::WorldPlanetChanged;
    // Also need old world position and velocity to properly initialize movement.
    const Variant & pn = eventData[ P_PLANET ];
    const PlanetBase * p_new = (PlanetBase *)pn.GetVoidPtr();

    // Get dynamics world.
    if ( !physicsWorld )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("PhysicsWorld2"), true );
        if ( !c )
            return;
        physicsWorld = SharedPtr<Component>( c );
    }
    if ( !worldMover )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("WorldMover"), true );
        if ( !c )
            return;
        worldMover = SharedPtr<Component>( c );
    }

    PhysicsWorld2 * pw2 = physicsWorld->Cast<PhysicsWorld2>();
    WorldMover    * wm  = worldMover->Cast<WorldMover>();
    if (p_new == this)
        initCollisions( pw2, wm, GameData::DIST_PLANET_COLLISIONS );
    else
        finitCollisions( pw2 );
}

void PlanetBase::OnWorldAdjusted( StringHash eventType, VariantMap & eventData )
{
    if ( !worldMover )
    {
        Scene * s = GetScene();
        Component * c = s->GetComponent( StringHash("WorldMover"), true );
        if ( !c )
            return;
        worldMover = SharedPtr<Component>( c );
    }

    if ( !physicsWorld )
      return;

    PhysicsWorld2 * pw2 = physicsWorld->Cast<PhysicsWorld2>();
    
    WorldMover * wm = worldMover->Cast<WorldMover>();
    if ( wm->planet != this )
      return;

    updateCollisions( pw2, wm, GameData::DIST_PLANET_COLLISIONS );
}




}


