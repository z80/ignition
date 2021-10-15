
#include "launch_site.h"
#include "game_data.h"
#include "player.h"

namespace Osp
{

LaunchSite::LaunchSite( Context * ctx )
    : ItemBase( ctx )
{

}

LaunchSite::~LaunchSite()
{

}

void LaunchSite::Start()
{
    Node * n = GetNode();
    /*ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "Prefabs/Site.xml" );
    if ( !f )
        return;
    Node * rootNode = n->CreateChild( "LaunchSiteVisualNode" );
    const bool loadedOk = rootNode->LoadXML( f->GetRoot() );
    if ( !loadedOk )
        URHO3D_LOGERROR( "Failed to load launch site prefab" );*/


    StaticModel * m = n->CreateComponent<StaticModel>();

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    m->SetModel( cache->GetResource<Model>( "Models/Site.mdl" ) );
    m->SetMaterial( cache->GetResource<Material>( "Materials/Stone.xml" ) );
    m->SetCastShadows( true );
}

void LaunchSite::createPlayer()
{
    Node * n = GetNode();
    Node * playerNode = n->CreateChild( "PlayerNode" );
    Player * pl = playerNode->CreateComponent<Player>();
}


}


