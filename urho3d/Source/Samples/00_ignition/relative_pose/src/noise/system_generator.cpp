
#include "system_generator.h"
#include "pi_system_generator.h"
#include "pi_source.h"
#include "pi_consts.h"


#include "sphere_dynamic.h"
#include "rotating_frame.h"
#include "orbiting_frame.h"

#include "settings.h"

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
    if ( false )
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
	//generator.apply( &s, rand );
    generator.createDumb( &s, rand );
}

void SystemGenerator::createBody( Scene * scene, RefFrame * parent, PiSystem & s, int bodyIndex )
{
    const PiSourceDesc & sbody = s.bodies_[ bodyIndex ];

    if ( sbody.super_type_ != SUPERTYPE_STAR )
    {
        PiBodySource * src = nullptr;
        if ( sbody.super_type_ == SUPERTYPE_STAR )
            src = PiBodySource::InstanceStar( sbody );
        else
            src = PiBodySource::InstanceTerrain( sbody );

        OrbitingFrame * of = scene->CreateComponent<OrbitingFrame>( REPLICATED );
        Float m = sbody.mass() * G;
        m *= 1.0e-12;
        of->SetGM( m );

        RotatingFrame * rf = scene->CreateComponent<RotatingFrame>( REPLICATED );
        //const Float periodDays = sbody.rotation_period_.ToDouble();
        //const Float periodSecs = periodDays * 86400.0;
        //rf->SetPeriod( Settings::ticks( periodSecs ) );
        rf->SetPeriod( Settings::ticks( 60.0 ) );
        rf->setParent( of );

        SphereDynamic * sd = scene->CreateComponent<SphereDynamic>( REPLICATED );
        sd->setParent( rf );
        sd->setHeightSource( src );
        const Float R = src->m_planetRadius;
        const Float H = src->m_maxHeightInMeters;
        sd->setRadius( 10.0, H/R*10.0 );
        sd->setStar( false );  // This sets the star material which is supposed to ignore lighing.
        sd->subdriveLevelsInit();

        static Float at = 0.0;
        if ( parent )
        {
            //of->setParent( parent );
        }
        sd->setR( Vector3d( at, 0.0, 0.0 ) );
        at += 20.0;

        //if ( at > 100.0 )
        //    return;

    }


    const unsigned qty = sbody.child_inds_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const int childIndex = sbody.child_inds_[i];
        const PiSourceDesc & b = s.bodies_[childIndex];
        if ( b.type_ != TYPE_PLANET_ASTEROID )
            //createBody( scene, sd, s, childIndex );
            createBody( scene, nullptr, s, childIndex );
    }
}

}






