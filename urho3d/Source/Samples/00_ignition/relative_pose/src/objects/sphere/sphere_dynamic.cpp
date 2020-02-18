
#include "sphere_dynamic.h"

namespace Ign
{

void SphereDynamic::RegisterComponent( Context * context )
{
    context->RegisterFactory<SphereDynamic>();
    URHO3D_COPY_BASE_ATTRIBUTES( SphereItem );
}

SphereDynamic::SphereDynamic( Context * context )
    : SphereItem( context )
{
    height_source_ = nullptr;
    R_ = 10.0;
}

SphereDynamic::~SphereDynamic()
{
    if ( height_source_ )
        delete height_source_;
}

void SphereDynamic::setRadius( Float r )
{
    R_ = r;
}

void SphereDynamic::setHeightSource( HeightSource * src )
{
    height_source_ = src;
}

void SphereDynamic::subdriveLevelsInit()
{
    cubesphereCollision_.setR( R_ );
    cubesphereVisual_.setR( R_ );

    subdriveSourceCollision_.clearLevels();
    subdriveSourceCollision_.addLevel( 1.0, 3.0 );
    //subdriveSourceCollision_.addLevel( 1.5, 8.0 );
    subdriveSourceCollision_.addLevel( 5.5, 18.0 );

    subdriveSourceVisual_.clearLevels();
    subdriveSourceVisual_.addLevel( 0.25, 6.0 );
    subdriveSourceVisual_.addLevel( 1.5, 18.0 );
}

void SphereDynamic::applySourceCollision( Cubesphere & cs )
{
    cs.applySource( height_source_ );
}

void SphereDynamic::applySourceVisual( Cubesphere & cs )
{
    cs.applySource( height_source_ );
}




}










