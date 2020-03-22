
#include "system_generator.h"
#include "pi_system_generator.h"
#include "pi_atmosphere_source.h"
#include "pi_source.h"
#include "pi_consts.h"
#include "pi_orbit.h"


#include "sphere_dynamic.h"
#include "rotating_frame.h"
#include "orbiting_frame.h"

#include "settings.h"

namespace Ign
{

void SystemGenerator::RegisterObject( Context * context )
{
    context->RegisterSubsystem<SystemGenerator>();
}

SystemGenerator::SystemGenerator( Context * context )
    : Urho3D::Object( context ), 
    s_( 10, 2, 1 )
{
    generateSystem();
}

SystemGenerator::~SystemGenerator()
{
}

void SystemGenerator::generate()
{
    generateSystem();
}

void SystemGenerator::createBodies( Scene * scene )
{
    createBody( scene, nullptr, s_, s_.root_body_ind_ );
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
            //PiRandom rand( seed );
            //PiSystem s( 10, 2, 1 );
            generateSystem();
            bool success = false;
            if ( s_.bodies_[0].type_ == TYPE_STAR_G )
            {
                // Check if there is Earth-like planet.
                const unsigned qty = s_.bodies_.Size();
                for ( unsigned i=1; i<qty; i++ )
                {
                    if ( s_.bodies_[i].type_ == TYPE_PLANET_TERRESTRIAL )
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



	generateSystem();

    createBody( scene, nullptr, s_, s_.root_body_ind_ );
}

void SystemGenerator::generateSystem()
{
	PiRandom rand;
	rand.seed( UNIVERSE_SEED );
	PiSystemGenerator generator;
	//generator.apply( &s, rand );
    generator.createDumb( &s_, rand );
}

void SystemGenerator::createBody( Scene * scene, RefFrame * parent, PiSystem & s, int bodyIndex )
{
    const PiSourceDesc & sbody = s.bodies_[ bodyIndex ];


    OrbitingFrame * of = scene->CreateComponent<OrbitingFrame>( REPLICATED );
    Float m = sbody.mass();
    m *= G;
    //m *= 1.0e-8;// / 30.0;
    of->SetGM( m );
    // Creating orbit.
    if ( bodyIndex != s_.root_body_ind_ )
    {
        PiOrbit po;
        const Float semimajorAxis = sbody.semimajor_axis_.ToDouble();
        OrbitingFrame * parentOf = parent->Cast<OrbitingFrame>();
        const Float totalMass = ( parentOf ) ? (parentOf->GM() / G) : (EARTH_MASS * 1.0e-8);
        const Float ecc = sbody.eccentricity_.ToDouble();
        po.SetShapeAroundPrimary( semimajorAxis, totalMass, ecc );
        const Float startPhase = sbody.start_eccentric_anomaly_.ToDouble();
        po.SetPhase( startPhase );

        const Quaterniond Qy( sbody.Y_, Vector3d::UP );
        const Quaterniond Qx( sbody.X_, Vector3d::LEFT );
        const Quaterniond Q = Qy * Qx;
        
        const Matrix3d m( Q.RotationMatrix() );
        po.SetPlane( m );

        const Vector3d R0 = po.OrbitalPosAtTime( 0.0 );
        const Vector3d V0 = po.OrbitalVelocityAtTime( totalMass, 0.0 );

        of->Launch( R0, V0 );
    }

    RotatingFrame * rf = scene->CreateComponent<RotatingFrame>( REPLICATED );
    const Float periodDays = sbody.rotation_period_.ToDouble();
    const Float periodSecs = periodDays * 86400.0;
    rf->SetPeriod( Settings::ticks( periodSecs ) );
    //rf->SetPeriod( Settings::ticks( 6000.0 ) );
    rf->setParent( of );


    SphereDynamic * sd = scene->CreateComponent<SphereDynamic>( REPLICATED );
    sd->setParent( rf );
    sd->SetBodyIndex( bodyIndex );

    static Float at = 0.0;
    if ( parent )
    {
        of->setParent( parent );
    }
    //sd->setR( Vector3d( at, 0.0, 0.0 ) );
    //at += 20.0;

    //if ( at > 100.0 )
    //    return;




    const unsigned qty = sbody.child_inds_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const int childIndex = sbody.child_inds_[i];
        const PiSourceDesc & b = s.bodies_[childIndex];
        //if ( b.type_ != TYPE_PLANET_ASTEROID )
        createBody( scene, of, s, childIndex );
        //createBody( scene, nullptr, s, childIndex );
    }
}

void SystemGenerator::applyBody( SphereDynamic * sd )
{
    const int bodyIndex = sd->GetBodyIndex();
    if ( bodyIndex < 0 )
        return;

    const PiSourceDesc & sbody = s_.bodies_[ bodyIndex ];

    PiBodySource * src = nullptr;
    if ( sbody.super_type_ == SUPERTYPE_STAR )
        src = PiBodySource::InstanceStar( sbody );
    else
        src = PiBodySource::InstanceTerrain( sbody );

    PiAtmosphereSource * atmosphereSource = new PiAtmosphereSource();
    *atmosphereSource = sbody;

    sd->setHeightSource( src );
    sd->setAtmosphereSource( atmosphereSource );
    const Float R = src->m_planetRadius;
    const Float H = src->m_maxHeightInMeters;
    sd->setRadius( R, H );
    //sd->setRadius( 100.0, H/R*100.0 );
    //sd->setRadius( 1000.0, 50000.0 );
    const bool isStar = (sbody.super_type_ == SUPERTYPE_STAR );
    sd->setStar( isStar );  // This sets the star material which is supposed to ignore lighing.
    sd->subdriveLevelsInit();
}

}






