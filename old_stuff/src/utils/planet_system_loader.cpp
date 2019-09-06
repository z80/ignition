
#include "planet_system_loader.h"
#include "planet_cs.h"

namespace Osp
{

bool PlanetSystemLoader::load( GameData * gd, Scene * s )
{
    Context * ctx = s->GetContext();
    URHO3D_LOGINFO( "Loading planets hierarchy" );
    JSONFile json( ctx );
    FileSystem * fs = ctx->GetSubsystem<FileSystem>();
    const String stri = fs->GetProgramDir() + "Data/Planets/planets.json";
    const bool loaded = json.LoadFile( stri );
    if ( !loaded )
        URHO3D_LOGINFO( "Failed to load JSON file" );
    const JSONValue & root = json.GetRoot();
    if ( !root.IsObject() )
        URHO3D_LOGINFO( "JSON file doesn't contain proper structure" );

    for ( ConstJSONObjectIterator it=root.Begin(); it!=root.End(); it++ )
    {
        const String & name = it->first_;
        const JSONValue & subitems = it->second_;
        const bool ok = loadPlanet( name, nullptr, subitems, gd, s );
        if ( !ok )
            return false;
    }

    return true;
}

bool PlanetSystemLoader::loadPlanet( const String & name, PlanetBase * parent, const JSONValue & v, GameData * gd, Scene * s )
{
    // First try open planet config file.
    // If it can be open create planet and feed in config file name.
    {
        Context * ctx = s->GetContext();
        URHO3D_LOGINFO( "Loading planet " + name );
        JSONFile json( ctx );
        FileSystem * fs = ctx->GetSubsystem<FileSystem>();
        const String stri = fs->GetProgramDir() + "Data/Planets/" + name + ".json";
        const bool loaded = json.LoadFile( stri );
        if ( !loaded )
        {
            URHO3D_LOGINFO( "Failed to load JSON file" );
            return false;
        }
        const JSONValue & root = json.GetRoot();
        if ( !root.IsObject() )
        {
            URHO3D_LOGINFO( "JSON file doesn't contain proper structure" );
            return false;
        }
    }
    // If parent exists create it as a child.
    // Alternatively create child of Scene.
    Node * n;
    if ( parent )
    {
        Node * pn = parent->GetNode();
        n = pn->CreateChild( name + "Node" );
    }
    else
        n = s->CreateChild( name + "Node" );

    PlanetCs * cs = n->CreateComponent<PlanetCs>();
    cs->setup( "Data/Planets/" + name + ".json" );
    cs->initParameters();
    gd->planets.Push( cs );
    // Debug variable.
    gd->planetDbg = cs;

    // Recursively parse children.
    for ( ConstJSONObjectIterator it=v.Begin(); it!=v.End(); it++ )
    {
        const String & childName = it->first_;
        const JSONValue & subitems = it->second_;
        const bool ok = loadPlanet( childName, cs, subitems, gd, s );
        if ( !ok )
            return false;
    }

    return true;
}








bool PlanetLoader::loadGeometry( const JSONValue & v, PlanetCs * p )
{
    ResourceCache * cache = p->GetSubsystem<ResourceCache>();

    {
        if ( !v.Contains( "heightmap" ) )
            URHO3D_LOGERROR( "Planet loader error: no heightmap specified" );
        const JSONValue & v_hm = v.Get( "heightmap" );
        const String & heightmapResourceName = v_hm.GetString();
        p->heightmap = cache->GetResource<Image>( heightmapResourceName );
        assert( p->heightmap != nullptr );
    }
    {
        if ( !v.Contains( "colormap" ) )
            URHO3D_LOGERROR( "Planet loader error: no colormap specified" );
        const JSONValue & v_cm = v.Get( "colormap" );
        const String & colormapResourceName = v_cm.GetString();
        p->colormap = cache->GetResource<Image>( colormapResourceName );
        assert( p->colormap != nullptr );
    }
    {
        if ( !v.Contains( "height_scale" ) )
            URHO3D_LOGERROR( "Planet loader error: no height scale specified" );
        const JSONValue & v_hs = v.Get( "height_scale" );
        p->heightScale = v_hs.GetDouble();
    }
    {
        if ( !v.Contains( "subdiv_max_sine" ) )
            URHO3D_LOGERROR( "Planet loader error: no max subdiv sine specified" );
        const JSONValue & v_hs = v.Get( "subdiv_max_sine" );
        p->subdivMaxSine = v_hs.GetDouble();
    }
    {
        if ( !v.Contains( "subdiv_max_level" ) )
            URHO3D_LOGERROR( "Planet loader error: no max subdiv level specified" );
        const JSONValue & v_hs = v.Get( "subdiv_max_level" );
        p->subdivMaxLevel = v_hs.GetInt();
    }
    return true;
}

bool PlanetLoader::loadKepler( const JSONValue & v, PlanetBase * p )
{
    Float GM, a, e, Omega, I, omega, E;
    const bool hasGM = v.Contains( "GM" );
    if ( hasGM )
    {
        const JSONValue & vv = v.Get( "GM" );
        GM = vv.GetDouble();
    }
    // Try retrieve Kepler GM from parent planet if it exists.
    {
        Node * n = p->GetNode();
        Node * pn = n->GetParent();
        const Vector<SharedPtr<Component> > & comps = pn->GetComponents();
        const unsigned qty = comps.Size();
        GM = -1.0;
        for ( unsigned i=0; i<qty; i++ )
        {
            Component * c = comps[i];
            PlanetBase * pb = c->Cast<PlanetBase>();
            if ( !pb )
                continue;
            GM = pb->GM();
            break;
        }
    }
    {
        assert( v.Contains( "a" ) );
        const JSONValue & vv = v.Get( "a" );
        a = vv.GetDouble();
    }
    {
        assert( v.Contains( "e" ) );
        const JSONValue & vv = v.Get( "e" );
        e = vv.GetDouble();
    }
    {
        assert( v.Contains( "Omega" ) );
        const JSONValue & vv = v.Get( "Omega" );
        Omega = vv.GetDouble();
    }
    {
        assert( v.Contains( "I" ) );
        const JSONValue & vv = v.Get( "I" );
        I = vv.GetDouble();
    }
    {
        assert( v.Contains( "omega" ) );
        const JSONValue & vv = v.Get( "omega" );
        omega = vv.GetDouble();
    }
    {
        assert( v.Contains( "E" ) );
        const JSONValue & vv = v.Get( "E" );
        E = vv.GetDouble();
    }
    if ( GM > 0.0 )
        p->mover->launch( GM, a, e, Omega, I, omega, E );
    else
        p->mover->active = false;
}

bool PlanetLoader::loadRotator( const JSONValue & v, PlanetBase * p )
{
    Float period, yaw, pitch, roll;
    {
        assert( v.Contains( "period" ) );
        const JSONValue & vv = v.Get( "period" );
        period = vv.GetDouble();
    }
    {
        assert( v.Contains( "yaw" ) );
        const JSONValue & vv = v.Get( "yaw" );
        yaw = vv.GetDouble();
    }
    {
        assert( v.Contains( "pitch" ) );
        const JSONValue & vv = v.Get( "pitch" );
        pitch = vv.GetDouble();
    }
    {
        assert( v.Contains( "roll" ) );
        const JSONValue & vv = v.Get( "roll" );
        roll = vv.GetDouble();
    }

    p->rotator->launch( period, yaw, pitch, roll );
    return true;
}

bool PlanetLoader::loadAssets( const String & fileName, PlanetCs * planet )
{
    Context * ctx = planet->GetContext();
    URHO3D_LOGINFO( "Loading asssets from " + fileName );
    JSONFile json( ctx );
    FileSystem * fs = ctx->GetSubsystem<FileSystem>();
    const String stri = fs->GetProgramDir() + fileName;
    const bool loaded = json.LoadFile( stri );
    if ( !loaded )
    {
        URHO3D_LOGINFO( "Failed to load JSON file" );
        return false;
    }
    const JSONValue & root = json.GetRoot();
    if ( !root.IsObject() )
    {
        URHO3D_LOGINFO( "JSON file doesn't contain proper structure" );
        return false;
    }

    for ( ConstJSONObjectIterator it=root.Begin(); it!=root.End(); it++ )
    {
        const JSONValue & v = it->second_;
        const bool ok = loadAsset( v, planet );
        if ( !ok )
        {
            const String & name = it->first_;
            URHO3D_LOGERROR( "Failed to load asset " + name );
            // Here don't return as it might be not super critical.
            //return false;
        }
    }

    return true;
}

bool PlanetLoader::loadAsset( const JSONValue & v, PlanetCs * planet )
{
    String tpe;
    Float lat, lon, az, h;
    {
        assert( v.Contains( "type" ) );
        const JSONValue & vv = v.Get( "type" );
        tpe = vv.GetString();
    }
    {
        assert( v.Contains( "lat" ) );
        const JSONValue & vv = v.Get( "lat" );
        lat = vv.GetDouble();
    }
    {
        assert( v.Contains( "lon" ) );
        const JSONValue & vv = v.Get( "lon" );
        lon = vv.GetDouble();
    }
    {
        assert( v.Contains( "az" ) );
        const JSONValue & vv = v.Get( "az" );
        az = vv.GetDouble();
    }
    {
        assert( v.Contains( "h" ) );
        const JSONValue & vv = v.Get( "h" );
        h = vv.GetDouble();
    }

    Node * root = planet->dynamicsNode;

    Node * n = root->CreateChild( tpe + "Node" );
    Component * c = n->CreateComponent( StringHash( tpe ) );
    if ( !c )
    {
        URHO3D_LOGERROR( "Failed to create " + tpe );
        n->Remove();
        return false;
    }
    ItemBase * t = c->Cast<ItemBase>();
    if ( !t )
    {
        URHO3D_LOGERROR( "Failed to cast to ItemBase " + tpe );
        t->Remove();
        n->Remove();
        return false;
    }

    // Compute position and orientation.
    const Float y = Sin( lat );
    const Float xz = Sqrt( 1.0 - y*y );
    const Float x = Cos( lon ) * xz;
    const Float z = Sin( lon ) * xz;
    const Float dh = planet->dh( Vector3d( x, y, z ) );
    const Float r = (1.0 + dh) * planet->forces->R_ + h;

    // "-" because here a quaternion rotates clockwise.
    const Quaterniond Qlon( -lon, Vector3d( 0.0, 1.0, 0.0 ) );
    const Quaterniond Qlat(  lat-90.0, Vector3d( 0.0, 0.0, 1.0 ) );
    const Quaterniond Qaz(  -az, Vector3d( 0.0, 1.0, 0.0 ) );

    const Quaterniond Q = Qlon * Qlat * Qaz;
    Vector3d at( 0.0, r, 0.0 );
    at = Q * at;
    t->setR( at );
    t->setQ( Q );

    return true;
}



}

