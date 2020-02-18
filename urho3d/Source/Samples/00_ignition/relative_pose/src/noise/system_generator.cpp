
#include "system_generator.h"
#include "pi_system_generator.h"
#include "pi_source.h"
#include "pi_consts.h"


#include "sphere_dynamic.h"

namespace Ign
{

SystemGenerator::SystemGenerator()
{
}

SystemGenerator::~SystemGenerator()
{
}

void SystemGenerator::generate( Scene * scene )
{
	PiSystem s( 10, 2, 1 );
	generateSystem( s );

    createBody( scene, nullptr, s, s.root_body_ind_ );
}

void SystemGenerator::generateSystem( PiSystem & s )
{
	PiRandom rand;
	rand.seed( UNIVERSE_SEED );
	PiSystemGenerator generator;
	generator.apply( &s, rand );
}

void SystemGenerator::createBody( Scene * scene, RefFrame * parent, PiSystem & s, int bodyIndex )
{
    const PiSourceDesc & sbody = s.bodies_[ bodyIndex ];
    
    PiBodySource * src = nullptr;
    if ( sbody.super_type_ == SUPERTYPE_STAR )
        src = PiBodySource::InstanceStar( sbody );
    else
        src = PiBodySource::InstanceTerrain( sbody );

    SphereDynamic * sd = scene->CreateComponent<SphereDynamic>( LOCAL );
    sd->setHeightSource( src );
    const Float R = sbody.radius_.ToDouble();
    sd->setRadius( 10.0 );
    sd->subdriveLevelsInit();

    static Float at = 0.0;
    if ( parent )
    {
        //sd->setParent( parent );
    }
    sd->setR( Vector3d( at, 0.0, 0.0 ) );
    at += 20.0;

    if ( at > 100.0 )
        return;


    const unsigned qty = sbody.child_inds_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const int childIndex = sbody.child_inds_[i];
        createBody( scene, sd, s, childIndex );
    }
}

}






