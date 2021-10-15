
#include "planet_cs.h"
#include "world_mover.h"

#include "physics_world_2.h"
#include "rigid_body_2.h"
#include "collision_shape_2.h"

#include "player.h"
#include "launch_site.h"

#include "planet_system_loader.h"

namespace Osp
{



PlanetCs::PlanetCs( Context * ctx )
    : PlanetBase( ctx )
{
    cg             = 0;
    rigidBody      = 0;
    collisionShape = 0;
    heightmap      = 0;
    colormap       = 0;
    initialized    = false;

    heightScale    = 0.03 / 255.0;
    subdivMaxLevel = 12;
    subdivMaxSine  = 0.1;

    configFileName = ""; //Data/Planets/cubesphere.json";
}

PlanetCs::~PlanetCs()
{
}

void PlanetCs::setup( const String & json )
{
    configFileName = json;
}

Float PlanetCs::dh( const Vector3d & at ) const
{
    const Vector2 uv = sphereCoord( at );
    const Float w = (Float)(heightmap->GetWidth()-1);
    const Float h = (Float)(heightmap->GetHeight()-1);
    const int ix = (int)(uv.x_ * w);
    const int iy = (int)(uv.y_ * h);
    const Color c = heightmap->GetPixel( ix, iy );
    const Float v = c.Average();
    const Float v_abs = heightScale * v;
    return v_abs;
}

bool  PlanetCs::needSubdrive( const Cubesphere::Cubesphere * s, const Cubesphere::Face * f ) const
{
    // Force some subdivision.
    //if ( f->level < 8 )
    //    return true;

    const bool maxLevelReached = (f->level >= subdivMaxLevel);
    if ( maxLevelReached )
        return false;

    Float maxSine = 0.0;
    for ( int i=0; i<4; i++ )
    {
        const int ind0 = i;
        int ind1 = i+1;
        if (ind1 > 3)
            ind1 = 0;
        Vector3d v0 = s->verts[ f->vertexInds[ind0] ].at - this->at;
        Vector3d v1 = s->verts[ f->vertexInds[ind1] ].at - this->at;
        v0.Normalize();
        v1.Normalize();
        const Vector3d cr = v0.CrossProduct( v1 );
        const Float d = std::abs( cr.Length() );
        if ( d > maxSine )
            maxSine = d;
    }
    for ( int i=0; i<2; i++ )
    {
        const int ind0 = i;
        int ind1 = i+2;
        if (ind1 > 3)
            ind1 = 0;
        Vector3d v0 = s->verts[ f->vertexInds[ind0] ].at - this->at;
        Vector3d v1 = s->verts[ f->vertexInds[ind1] ].at - this->at;
        v0.Normalize();
        v1.Normalize();
        const Vector3d cr = v0.CrossProduct( v1 );
        const Float d = std::abs( cr.Length() );
        if ( d > maxSine )
            maxSine = d;
    }
    const bool needSubdrive = ( maxSine >= subdivMaxSine );
    return needSubdrive;
}

void PlanetCs::Start()
{
    PlanetBase::Start();

    Node * n = dynamicsNode;
    cg = n->CreateComponent<CustomGeometry>();
    cg->SetDynamic( true );

    initParameters();



    /*LaunchSite * site;
    {
        // Launch site
        Node * n = SharedPtr<Node>( dynamicsNode->CreateChild( "LaunchSite" ) );
        //const Quaternion q = Quaternion( 70.0, Vector3( 0.0, 0.0, 1.0 ) );
        //const Vector3 at = q * Vector3( 0.0, 11.0, 0.0 );
        const Quaternion q = Quaternion( 0.0, Vector3( 0.0, 0.0, 1.0 ) );
        const Vector3 at = q * Vector3( 0.0, 1010.0, 0.0 );

        n->SetPosition( at );
        n->SetRotation( q );
        site = n->CreateComponent<LaunchSite>();

        {
            Scene * s = GetScene();
            Camera * c = s->GetComponent( StringHash( "Camera" ), true )->Cast<Camera>();
            Node * cn = c->GetNode();
            cn->SetParent( n );
        }
    }

    // Create a player.
    {
        Node * n = site->GetNode();
        Node * playerNode = n->CreateChild( "PlayerNode" );
        playerNode->CreateComponent<Player>();
    }*/
}

void PlanetCs::updateCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
    if ( !rigidBody )
        return;

    {
        const Float h = dh( Vector3d( 0.0, 1.0, 0.0 ) );
        int i = h;
    }

    updateGeometry( mover );

    Node * n = rigidBody->GetNode();
    Vector3d    rel_r;
    Quaterniond rel_q;
    rotator->relativePose( mover, rel_r, rel_q );
    //n->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    //n->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );
    rigidBody->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    rigidBody->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );

    collisionShape->SetCustomGImpactMesh( cg );
}

void PlanetCs::initCollisions( PhysicsWorld2 * w2, Osp::WorldMover * mover, Float dist )
{
    updateGeometry( mover );

    Node * worldNode = w2->GetNode();
    Node * n = worldNode->CreateChild( "PlanetCollisionShape" );

    rigidBody = n->CreateComponent<RigidBody2>();
    rigidBody->SetMass( 0.0 );

    collisionShape = n->CreateComponent<CollisionShape2>();
    collisionShape->SetCustomGImpactMesh( cg );
    //collisionShape->SetCustomTriangleMesh( cg );

    Vector3d    rel_r;
    Quaterniond rel_q;
    rotator->relativePose( mover, rel_r, rel_q );
    //n->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    //n->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );
    rigidBody->SetPosition( Vector3( rel_r.x_, rel_r.y_, rel_r.z_ ) );
    rigidBody->SetRotation( Quaternion( rel_q.w_, rel_q.x_, rel_q.y_, rel_q.z_ ) );
}

void PlanetCs::finitCollisions( PhysicsWorld2 * w2 )
{
    if ( collisionShape )
    {
        collisionShape->Remove();
        collisionShape = 0;
    }
    if ( rigidBody )
    {
        rigidBody->Remove();
        rigidBody = 0;
    }
}

bool PlanetCs::load( const JSONValue & root )
{
    if ( !PlanetBase::load( root ) )
        return false;

    {
        assert( root.Contains( "geometry" ) );
        const JSONValue & vv = root.Get( "geometry" );
        const bool ok = PlanetLoader::loadGeometry( vv, this );
    }
    // Load assets if they exist.
    if ( root.Contains( "assets" ) )
    {
        const JSONValue & vv = root.Get( "assets" );
        const String fileName = vv.GetString();
        const bool ok = PlanetLoader::loadAssets( fileName, this );
    }
    if ( root.Contains( "random_assets" ) )
    {
        const JSONValue & vv = root.Get( "random_assets" );
        const String fileName = vv.GetString();
        // Here need to implement loading random assets.
    }

    // Modify height scale.
    // Make it in percent of planet radius.
    // Color is normalized by [0.0, 1.0]. No need to divide by 255.0.
    // Normalize to planet radius as initially radius is 1 and multiplied
    // by actual radius later.
    heightScale = heightScale / forces->R_;

    return true;
}

void PlanetCs::initParameters( const String & fileName )
{
    // ".json".Length() = 5
    String fname;
    if ( fileName.Length() > 5 )
    {
        fname = fileName;
        configFileName = fileName;
    }
    else
    {
        if ( configFileName.Length() <= 5 )
            return;
        fname = configFileName;
    }
    URHO3D_LOGINFO( "Loading config file " + fname );
    JSONFile json( context_ );
    FileSystem * fs = GetSubsystem<FileSystem>();
    const String stri = fs->GetProgramDir() + fname;
    const bool loaded = json.LoadFile( stri );
    if ( !loaded )
        URHO3D_LOGINFO( "Failed to load JSON file" );
    const JSONValue & root = json.GetRoot();
    if ( !root.IsObject() )
        URHO3D_LOGINFO( "JSON file doesn't contain proper structure" );

    load( root );
    updateGeometry( Vector3d( 1.0, 0.0, 0.0 ) );
}

void PlanetCs::updateGeometry( Osp::WorldMover * mover )
{
    Vector3d    rel_r;
    Quaterniond rel_q;
    if ( mover )
    {
        mover->relativePose( rotator, rel_r, rel_q );
        rel_r.Normalize();
    }

    updateGeometry( rel_r );
}

void PlanetCs::updateGeometry( const Vector3d & at )
{
    setCameraAt( at );
    cubesphere.subdrive( this );
    cubesphere.triangleList( tris );

    const unsigned qty = tris.Size();

    cg->Clear();
    cg->SetNumGeometries( 1 );
    cg->BeginGeometry( 0, TRIANGLE_LIST );

    /*cg->DefineVertex( Vector3( 0.0, 1.0, 0.0 ) * forces->R_ );
    cg->DefineColor( Color( 1.0, 0.0, 0.0 ) );
    cg->DefineNormal( Vector3( 0.0, 1.0, 0.0 ) );
    cg->DefineTexCoord( Vector2( 0.0, 0.0 ) );

    cg->DefineVertex( Vector3( 1.0, 1.0, 0.0 ) * forces->R_ );
    cg->DefineColor( Color( 0.0, 1.0, 0.0 ) );
    cg->DefineNormal( Vector3( 0.0, 1.0, 0.0 ) );
    cg->DefineTexCoord( Vector2( 1.0, 0.0 ) );

    cg->DefineVertex( Vector3( 0.5, 1.0, 1.0 ) * forces->R_ );
    cg->DefineColor( Color( 0.0, 0.0, 1.0 ) );
    cg->DefineNormal( Vector3( 0.0, 1.0, 0.0 ) );
    cg->DefineTexCoord( Vector2( 1.0, 1.0 ) );*/

    const Float w = (Float)(heightmap->GetWidth()-1);
    const Float h = (Float)(heightmap->GetHeight()-1);
    for ( unsigned i=0; i<qty; i++ )
    {
        const Cubesphere::Vertex & v = tris[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );

        const Vector2 uv = sphereCoord( v.at );
        const Float x = uv.x_ * w;
        const Float y = uv.y_ * h;
        const int ix = (int)x;
        const int iy = (int)y;
        const Color c = colormap->GetPixel( ix, iy );

        cg->DefineVertex( at * forces->R_ );
        cg->DefineColor( c );
        cg->DefineNormal( n );
        cg->DefineTexCoord( uv );
    }

    cg->Commit();


    ResourceCache * cache = GetSubsystem<ResourceCache>();
    Material * m = cache->GetResource<Material>("Materials/VertexColor.xml");
    //Material * m = cache->GetResource<Material>("Materials/Stone.xml");
    cg->SetMaterial( m );
    cg->SetCastShadows( true );
}

Vector2 PlanetCs::sphereCoord( const Vector3d & at ) const
{
    const Float p = Sqrt( at.x_*at.x_ + at.z_* at.z_ );
    const Float lat = Atan2( at.y_, p );
    const Float lon = Atan2( at.z_, at.x_ );
    const Float x = (lon + 180.0) / 360.0;
    const Float y = (90.0 - lat) / 180.0;

    const Vector2 uv( x, y );
    return uv;
}






}




