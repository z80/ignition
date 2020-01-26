
#include "sphere_item.h"
#include "physics_frame.h"

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
    subdriveLevelsInit();
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

void SphereItem::subdriveLevelsInit()
{
    cubesphereCollision_.setR( 100.0 );
    cubesphereVisual_.setR( 100.0 );

    subdriveSourceCollision_.addLevel( 1.0, 30.0 );
    subdriveSourceCollision_.addLevel( 5.0, 80.0 );
    subdriveSourceCollision_.addLevel( 15.0, 280.0 );

    subdriveSourceVisual_.addLevel( 0.5, 30.0 );
    subdriveSourceVisual_.addLevel( 5.0, 80.0 );
    subdriveSourceVisual_.addLevel( 15.0, 280.0 );
}

void SphereItem::OnSceneSet( Scene * scene )
{
    if ( !scene )
        return;

    Node * n = scene->CreateChild();
    node_ = SharedPtr<Node>( n );

    CustomGeometry * cg = n->CreateComponent<CustomGeometry>();
    geometry_ = SharedPtr<CustomGeometry>( cg );

    // Here also need to assign a material to the geometry componenet.
}

void SphereItem::subdivideCollision()
{
    pts_.Clear();
    const Vector<SharedPtr<RefFrame> > & chs = children_;
    const unsigned qty = chs.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        RefFrame * rf = chs[i];
        if ( !rf )
            continue;
        PhysicsFrame * pf = rf->Cast<PhysicsFrame>();
        if ( !pf )
            continue;
        const Vector3d r = pf->relR();
        pts_.Push( r );
    }

    const bool need = subdriveSourceCollision_.needSubdrive( &cubesphereCollision_, pts_ );
    if ( need )
        cubesphereCollision_.subdrive( &subdriveSourceCollision_ );
}

void SphereItem::subdivideVisual()
{
    Context * context = GetContext();
    Environment * env = Environment::environment( context );
    if ( !env )
        return;

    pts_.Clear();
    // Get current client and find its physics environment.
    CameraFrame * cam = env->FindCameraFrame();
    State s;
    cam->relativePose( this, s );
    pts_.Push( s.r );

    const bool need = subdriveSourceVisual_.needSubdrive( &cubesphereVisual_, pts_ );
    if ( need )
    {
        cubesphereVisual_.subdrive( &subdriveSourceVisual_ );
        regenerateMesh();
    }
}

void SphereItem::regenerateMesh()
{

}







}






