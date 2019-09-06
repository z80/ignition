
#include "kepler_mover_planet.h"

namespace Osp
{

KeplerMoverPlanet::KeplerMoverPlanet( Context * ctx )
    : KeplerMover( ctx )
{

}

KeplerMoverPlanet::~KeplerMoverPlanet()
{

}

void KeplerMoverPlanet::Start()
{
    Node * n = GetNode();
    Vector< SharedPtr<Component> > comps;
    comps = n->GetComponents();
    const size_t qty = comps.Size();
    PlanetBase * p = nullptr;
    for ( size_t i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        PlanetBase * p = c->Cast<PlanetBase>();
        if ( p )
        {
            planet = SharedPtr<PlanetBase>( p );
            return;
        }
    }
}

void KeplerMoverPlanet::Update( float dt )
{
    KeplerMover::Update( dt );
    if ( !planet )
        return;
    const Vector3d r = relR();
    const Vector3d v = relV();
    planet->setR( r );
    planet->setV( v );
}

}

