
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
    H_ = 1.0;
}

SphereDynamic::~SphereDynamic()
{
    if ( height_source_ )
        delete height_source_;
}

void SphereDynamic::setRadius( Float r, Float h )
{
    R_ = r;
    H_ = h;
    subdriveLevelsInit();
}

void SphereDynamic::setHeightSource( HeightSource * src )
{
    height_source_ = src;
}

void SphereDynamic::setStar( bool isStar )
{
    if ( isStar )
        setMaterialName( "Ign/Materials/VertexColorStar.xml" );
    else
        setMaterialName( "Ign/Materials/VertexColor.xml" );
}

Vector3d SphereDynamic::surfacePos( const Vector3d & unitAt )
{
    const Float l = unitAt.Length();
    if ( l < 0.5 )
        return Vector3d::ZERO;
    if ( !height_source_ )
    {
        const Vector3d at = unitAt * R_ / l;
        return at;
    }
    const Float h = height_source_->height( unitAt );
    const Vector3d at = unitAt *( (R_ + h*H_) / l );
    return at;
}

void SphereDynamic::subdriveLevelsInit()
{
    cubesphereCollision_.setR( R_ );
    cubesphereCollision_.setH( H_ );
    cubesphereVisual_.setR( R_ );
    cubesphereVisual_.setH( H_ );

    subdriveSourceCollision_.clearLevels();
    subdriveSourceCollision_.addLevel( 1.0, 3.0 );
    //subdriveSourceCollision_.addLevel( 1.5, 8.0 );
    subdriveSourceCollision_.addLevel( 5.5, 18.0 );

    subdriveSourceVisual_.clearLevels();
    subdriveSourceVisual_.addLevel( 0.5, 6.0 );
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










