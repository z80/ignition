
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
    {
        for ( int i=0; i<100; i++ )
        {
            PiRandom rand( i * 1000000 + i );
            uint32_t v = rand.Int32( 128 );
            v = v + 0;
        }
    }
    {
        uint32_t seed = 0;
        for ( ;; )
        {
            PiRandom rand( seed );
            PiSystem s( 10, 2, 1 );
            generateSystem( s );
            bool success = false;
            if ( s.bodies_[0].type_ == TYPE_STAR_G )
            {
                // Check if there is Earth-like planet.
                const unsigned qty = s.bodies_.Size();
                for ( unsigned i=1; i<qty; i++ )
                {
                    if ( s.bodies_[i].type_ == TYPE_PLANET_TERRESTRIAL )
                    {
                        success = true;
                        break;
                    }
                }
            }
            if ( success )
                break;
            seed += 1;
        }
    }



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
    const Float R = src->m_planetRadius;
    const Float H = src->m_maxHeightInMeters;
    sd->setRadius( 10.0, H/R*10.0 );
    sd->setStar( true );
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
        const PiSourceDesc & b = s.bodies_[childIndex];
        if ( b.type_ != TYPE_PLANET_ASTEROID )
            createBody( scene, sd, s, childIndex );
    }
}

}






