
#include "test_environment.h"
#include "camera_frame.h"
#include "static_mesh.h"
#include "infinite_plane.h"
#include "dynamic_cube.h"
#include "character_cube.h"
#include "ico_planet.h"
#include "sphere_example.h"
#include "sphere_dynamic.h"
#include "surface_collision_mesh.h"
#include "rotating_frame.h"
#include "orbiting_frame.h"
#include "settings.h"

#include "Notifications.h"

#include "pi_system.h"
#include "pi_system_generator.h"
#include "system_generator.h"

namespace Ign
{

void TestEnvironment::RegisterComponent( Context * context )
{
    context->RegisterFactory<TestEnvironment>();
}

TestEnvironment::TestEnvironment( Context * context )
    : Environment( context )
{
}

TestEnvironment::~TestEnvironment()
{

}

void TestEnvironment::DelayedStart()
{

}

bool TestEnvironment::ClientConnected( int id, const VariantMap & identity, String & errMsg )
{

    return true;
}

void TestEnvironment::CreateReplicatedContentServer()
{
    contentServerPlanet();
}

void TestEnvironment::CreateReplicatedContentClient( CameraFrame * camera )
{
    contentClientCharacterCube( camera );
}

void TestEnvironment::contentServerTestSystem()
{
    Scene * s = GetScene();

    /*InfinitePlane * p = s->CreateComponent<InfinitePlane>();
    p->setName( "Infinite plane object" );
    p->setR( Vector3d::ZERO );
    p->setQ( Quaterniond::IDENTITY );*/

    StaticMesh * m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 10.0, 0.0, 0.0 ) );

    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 0.0, -10.0 ) );

    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 10.0, 0.0 ) );
    m = s->CreateComponent<StaticMesh>();
    m->setR( Vector3d( 0.0, 12.0, 0.0 ) );

    /*IcoPlanet * ip = s->CreateComponent<IcoPlanet>();
    ip->setR( Vector3d( 0.0, 0.0, 20.0 ) );*/

    {
        OrbitingFrame * of = s->CreateComponent<OrbitingFrame>();
        of->SetGM( 50.0 );
        of->setR( Vector3d( 0.0, 0.0, 80.0 ) );

        RotatingFrame * rf = s->CreateComponent<RotatingFrame>();
        rf->setParent( of );
        rf->setR( Vector3d::ZERO );
        rf->SetPeriod( 10 * Settings::ticksPerSec() );

        IcoPlanet * ip = s->CreateComponent<IcoPlanet>();
        ip->setParent( rf );
        ip->setR( Vector3d::ZERO );

        //SphereExample * se = s->CreateComponent<SphereExample>();
        //se->setR( Vector3d( 10.0, 0.0, 0.0 ) );

        // Create orbiting element.
        {
            OrbitingFrame * of2 = s->CreateComponent<OrbitingFrame>();
            of2->setParent( of );
            of2->setR( Vector3d( 0.0, 20.0, 0.0 ) );
            const bool launchOk = of2->Launch( Vector3d( 0.0, 20.0, 0.0 ), Vector3d( 1.0, 0.0, 0.0 ) );
            const String stri = String( "Launch ok: " ) + String( launchOk ? "true" : "false" );
            Notifications::AddNotification( GetContext(), stri );

            ip = s->CreateComponent<IcoPlanet>();
            ip->setParent( of2 );
            ip->setR( Vector3d::ZERO );
        }

        // Create generated system.
        {
            PiRandom rand;
            SystemGenerator generator;
            generator.generate( s );
        }
    }
}

void TestEnvironment::contentClientCameraOnly( CameraFrame * camera )
{
    Scene * s = GetScene();
    if ( !s )
        return;

    //DynamicCube * d = s->CreateComponent<DynamicCube>();
    //d->setName( String( "DynamicCube object" ) );

    //CharacterCube * d = s->CreateComponent<CharacterCube>();
    //d->setName( String( "CharacterCube object" ) );
    //d->setR( Vector3d( 0.0, 5.0, 0.0 ) );
    //camera->setParent( d );

    {
        Scene * s = GetScene();
        const Vector<SharedPtr<Component> > & comps = s->GetComponents();
        const unsigned qty = comps.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            Component * c = comps[i];
            if ( !c )
                continue;
            SphereItem * si = c->Cast<SphereItem>();
            if ( !si )
                continue;

            camera->setParent( si );
            break;
        }
    }

    // Parent surfaceCollisionMesh to CameraFrame
    // to be able to change its position on demand.
    //SurfaceCollisionMesh * scm = s->CreateComponent<SurfaceCollisionMesh>();
    //scm->setParent( camera );
    //scm->setR( Vector3d::ZERO );
}

void TestEnvironment::contentServerPlanet()
{
    Scene * s = GetScene();
    if ( !s )
        return;

    // Create generated system.
    {
        PiRandom rand;
        SystemGenerator generator;
        generator.generate( s );
    }
}

void TestEnvironment::contentClientCharacterCube( CameraFrame * camera )
{
    Scene * s = GetScene();
    if ( !s )
        return;

    DynamicCube * d = s->CreateComponent<DynamicCube>();
    d->setName( String( "DynamicCube object" ) );

    CharacterCube * cc = s->CreateComponent<CharacterCube>();
    cc->setName( String( "CharacterCube object" ) );
    cc->setR( Vector3d( 0.0, 5.0, 0.0 ) );
    camera->setParent( cc );

    // Search for a planet and place everything on a side.
    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    SphereDynamic * planet = nullptr;
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        planet = c->Cast<SphereDynamic>();
        if ( planet )
            break;
    }
    if ( !planet )
        return;

    d->setParent( planet );
    cc->setParent( planet );
    {
        const Vector3d at = planet->surfacePos( Vector3d( 1.0, 0.0, 0.0 ), 2.0 );
        d->setR( at );
    }
    {
        const Vector3d at = planet->surfacePos( Vector3d( 1.0, 0.0, 0.0 ), 3.5 );
        cc->setR( at );
    }

}




}





