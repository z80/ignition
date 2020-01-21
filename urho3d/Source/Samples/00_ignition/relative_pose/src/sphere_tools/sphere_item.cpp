
#include "sphere_item.h"

namespace Ign
{

void SphereItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<SphereItem>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );

}

SphereItem::SphereItem( Context * context )
    : RefFrame( context )
{

}

SphereItem::~SphereItem()
{

}

void SphereItem::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{

}

void SphereItem::updateData()
{
    subdivideCollision();
}

void SphereItem::OnSceneSet( Scene * scene )
{

}

void SphereItem::subdivideCollision()
{

}

void SphereItem::subdivideVisual()
{

}


}






