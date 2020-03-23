
#include "sphere_dynamic.h"
#include "physics_item.h"
#include "system_generator.h"

namespace Ign
{

void SphereDynamic::RegisterComponent( Context * context )
{
    context->RegisterFactory<SphereDynamic>();
    URHO3D_COPY_BASE_ATTRIBUTES( SphereItem );
    URHO3D_ACCESSOR_ATTRIBUTE( "BodyIndex", GetBodyIndex, SetBodyIndex, int, -1, AM_DEFAULT );
}

SphereDynamic::SphereDynamic( Context * context )
    : SphereItem( context )
{
    body_index_        = -1;
    height_source_     = nullptr;
    atmosphere_source_ = nullptr;
    R_ = 10.0;
    H_ = 1.0;
}

SphereDynamic::~SphereDynamic()
{
    if ( height_source_ )
        delete height_source_;
    if ( atmosphere_source_ )
        delete atmosphere_source_;
}

bool SphereDynamic::Recursive() const
{
    return true;
}

bool SphereDynamic::ProducesForces() const
{
    const bool ok = ( atmosphere_source_ != nullptr );
    return ok;
}

void SphereDynamic::ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const
{
    AtmosphereSource * s = atmosphere_source_;
    if ( !s )
        return;
    AirMesh & a = receiver->airMesh();
    s->drag( a, st, F, P );
}

int  SphereDynamic::GetBodyIndex() const
{
    return body_index_;
}

void SphereDynamic::SetBodyIndex( int index )
{
    if ( index == body_index_ )
        return;
    body_index_ = index;
    initPiSurface();

    MarkNetworkUpdate();
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

void SphereDynamic::setAtmosphereSource( AtmosphereSource * src )
{
    atmosphere_source_ = src;
}

void SphereDynamic::setStar( bool isStar )
{
    if ( isStar )
        setMaterialName( "Ign/Materials/VertexColorStar.xml" );
    else
        setMaterialName( "Ign/Materials/VertexColor.xml" );
}

Vector3d SphereDynamic::surfacePos( const Vector3d & unitAt, const Float height )
{
    const Float l = unitAt.Length();
    if ( l < 0.5 )
        return Vector3d::ZERO;
    if ( !height_source_ )
    {
        const Vector3d at = unitAt * ( (R_ + height) / l );
        return at;
    }
    const Float h = height_source_->height( unitAt );
    const Vector3d at = unitAt *( (R_ + h*H_ + height) / l );
    return at;
}

void SphereDynamic::subdriveLevelsInit()
{
    cubesphereCollision_.setR( R_ );
    cubesphereCollision_.setH( H_ );
    cubesphereVisual_.setR( R_ );
    cubesphereVisual_.setH( H_ );

    subdriveSourceCollision_.clearLevels();
    subdriveSourceCollision_.addLevel( 70.0, 350.0 );
    //subdriveSourceCollision_.addLevel( 1.5, 8.0 );
    //subdriveSourceCollision_.addLevel( 200.5, 380.0 );

    subdriveSourceVisual_.clearLevels();
    //subdriveSourceVisual_.addLevel( 0.3, 26.0 );
    subdriveSourceVisual_.addLevel( 5.0, 100.0 );
    subdriveSourceVisual_.addLevel( 50.0, 1000.0 );
    subdriveSourceVisual_.addLevel( 500.0, 10000.0 );
}

void SphereDynamic::initPiSurface()
{
    if ( height_source_ )
        delete height_source_;
    if ( atmosphere_source_ )
        delete atmosphere_source_;
    height_source_     = nullptr;
    atmosphere_source_ = nullptr;

    SystemGenerator * generator = context_->GetSubsystem<SystemGenerator>();
    generator->applyBody( this );
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










