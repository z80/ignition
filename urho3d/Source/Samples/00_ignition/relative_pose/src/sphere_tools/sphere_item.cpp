
#include "sphere_item.h"
#include "physics_frame.h"
#include "camera_frame.h"
#include "settings.h"

namespace Ign
{

static void workFuncCollision( const WorkItem * item, unsigned index );
static void workFuncVisual( const WorkItem * item, unsigned index );


void SphereItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<SphereItem>();
    URHO3D_COPY_BASE_ATTRIBUTES( ForceSourceFrame );
}

SphereItem::SphereItem( Context * context )
    : ForceSourceFrame( context )
{
    material_ = "Ign/Materials/VertexColor.xml";
    setName( "SphereItem" );

    visualUpdateNeeded_         = false;
    visualUpdateRunning_        = false;
    visualUpdateToBeApplied_    = false;

    collisionUpdateNeeded_      = false;
    collisionUpdateRunning_     = false;
    collisionUpdateToBeApplied_ = false;

    valid_ = false;
}

SphereItem::~SphereItem()
{

}

void SphereItem::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{

}

void SphereItem::updateCollisionData()
{
    //subdivideCollision();
    checkIfSubdriveCollisionNeeded();
}

void SphereItem::updateVisualData()
{
    //subdivideVisual();
    checkIfSubdriveVisualNeeded();
}

void SphereItem::refStateChanged()
{
    const Vector3    r = refR().vector3();
    const Quaternion q = refQ().quaternion();    

    // Check the distance. If it is beyond reasonable visibility range 
    // specified in Settings make it closer and scale it down.
    const Float maxDistance = Settings::cameraMaxDistance();
    Float dist = r.Length();
    if ( dist > maxDistance )
    {
        Float scale = maxDistance / dist;
        Vector3 scaledR = r * scale;
        node_->SetScale( scale );
        node_->SetTransform( scaledR, q );
    }
    else
    {
        node_->SetScale( 1.0 );
        node_->SetTransform( r, q );
    }

    finishSubdriveCollision();
    startSubdriveCollision();

    finishSubdriveVisual();
    startSubdriveVisual();
}

void SphereItem::poseChanged()
{
    computeRefState( nullptr );
    URHO3D_LOGINFOF( "SphereItem::poseChanged()" );
}

void SphereItem::subdriveLevelsInit()
{
    cubesphereCollision_.setR( 10.0 );
    cubesphereVisual_.setR( 10.0 );

    subdriveSourceCollision_.addLevel( 0.5, 3.0 );
    //subdriveSourceCollision_.addLevel( 1.5, 8.0 );
    subdriveSourceCollision_.addLevel( 5.5, 18.0 );

    subdriveSourceVisual_.addLevel( 0.1, 1.0 );
    //subdriveSourceVisual_.addLevel( 1.5, 8.0 );
    subdriveSourceVisual_.addLevel( 5.5, 18.0 );
}

void SphereItem::setMaterialName( const String & material )
{
    material_ = material;
}

bool SphereItem::valid() const
{
    const bool res = valid_;
    return res;
}

void SphereItem::OnSceneSet( Scene * scene )
{
    if ( !scene )
        return;

    subdriveLevelsInit();

    Node * n = scene->CreateChild( String( "SphereItem" ), LOCAL );
    node_ = SharedPtr<Node>( n );
    // To be able to get RefFrame by Node.
    assignRefFrame( node_ );

    CustomGeometry * cg = n->CreateComponent<CustomGeometry>( LOCAL );
    geometry_ = SharedPtr<CustomGeometry>( cg );

    // No, material need to be assigned each time geometry is changed.
    // Otherwise it is default material for some reason.

    /*// Here also need to assign a material to the geometry componenet.
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    Material * m = cache->GetResource<Material>("Ign/Materials/VertexColor.xml");
    //Material * m = cache->GetResource<Material>("Materials/Stone.xml");
    m->SetFillMode( FILL_WIREFRAME );
    cg->SetMaterial( m );
    cg->SetCastShadows( true );
    cg->SetDynamic( true );*/
}

void SphereItem::applySourceCollision( Cubesphere & cs )
{

}

void SphereItem::applySourceVisual( Cubesphere & cs )
{

}

void SphereItem::subdivideCollision()
{
    ptsCollision_.Clear();
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
        SubdriveSource::SubdividePoint pt;
        pt.close = true;
        pt.at = pf->relR();
        ptsCollision_.Push( pt );
    }

    const bool need = subdriveSourceCollision_.needSubdrive( &cubesphereCollision_, ptsCollision_ );
    if ( need )
    {
        cubesphereCollision_.subdrive( &subdriveSourceCollision_ );
        applySourceCollision( cubesphereCollision_ );
    }
}


void SphereItem::checkIfSubdriveCollisionNeeded()
{
    ptsCollision_.Clear();
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
        SubdriveSource::SubdividePoint pt;
        pt.at = pf->relR();
        pt.close = true;
        ptsCollision_.Push( pt );
    }
    const bool need = subdriveSourceCollision_.needSubdrive( &cubesphereCollision_, ptsCollision_ );
    if ( !need )
        return;

    collisionUpdateNeeded_ = true;
}

void SphereItem::startSubdriveCollision()
{
    if ( !collisionUpdateNeeded_ )
        return;
    if ( collisionUpdateRunning_ )
        return;

    WorkQueue * wq = GetSubsystem<WorkQueue>();
    SharedPtr<WorkItem> wi = wq->GetFreeItem();

    collisionUpdateNeeded_      = false;
    collisionUpdateRunning_     = true;
    collisionUpdateToBeApplied_ = false;

    cubesphereCollisionTh_ = cubesphereCollision_;

    wi->priority_ = 0;
    wi->aux_ = this;
    wi->workFunction_ = workFuncCollision;
    wq->AddWorkItem( wi );
}

void SphereItem::processSubdriveCollision()
{
    cubesphereCollisionTh_.subdrive( &subdriveSourceCollision_ );
    applySourceCollision( cubesphereCollisionTh_ );

    collisionUpdateRunning_ = false;
    collisionUpdateToBeApplied_ = true;
}

void SphereItem::finishSubdriveCollision()
{
    if ( collisionUpdateRunning_ )
        return;
    if ( !collisionUpdateToBeApplied_ )
        return;
    collisionUpdateToBeApplied_ = false;
    cubesphereCollision_ = cubesphereCollisionTh_;

    valid_ = true;
    using namespace SphereCollisionUpdated;
    VariantMap & eventData = GetEventDataMap();
    const unsigned id = GetID();
    eventData[P_REF_FRAME_ID] = id;
    SendEvent( E_SPHERE_COLLISION_UPDATED, eventData );
}


void SphereItem::subdivideVisual()
{
    Context * context = GetContext();
    Environment * env = Environment::environment( context );
    if ( !env )
        return;

    ptsVisual_.Clear();
    // Get current client and find its physics environment.
    CameraFrame * cam = env->FindCameraFrame();
    State s;
    cam->relativeState( this, s, true );
    SubdriveSource::SubdividePoint pt;
    pt.at = s.r;
    pt.close = true;
    ptsVisual_.Push( pt );

    const bool need = subdriveSourceVisual_.needSubdrive( &cubesphereVisual_, ptsVisual_ );
    if ( need )
    {
        cubesphereVisual_.subdrive( &subdriveSourceVisual_ );
        applySourceVisual( cubesphereVisual_ );
        regenerateMeshVisual();
    }
}

void SphereItem::checkIfSubdriveVisualNeeded()
{
    Context * context = GetContext();
    Environment * env = Environment::environment( context );
    if ( !env )
        return;

    ptsVisual_.Clear();
    // Get current client and find its physics environment.
    CameraFrame * cam = env->FindCameraFrame();
    State s;
    cam->relativeState( this, s, true );
    SubdriveSource::SubdividePoint pt;
    pt.at = s.r;
    pt.close = true;
    ptsVisual_.Push( pt );

    const bool need = subdriveSourceVisual_.needSubdrive( &cubesphereVisual_, ptsVisual_ );
    if ( !need )
        return;

    visualUpdateNeeded_ = true;
}

void SphereItem::startSubdriveVisual()
{
    if ( !visualUpdateNeeded_ )
        return;
    if ( visualUpdateRunning_ )
        return;
    WorkQueue * wq = GetSubsystem<WorkQueue>();
    SharedPtr<WorkItem> wi = wq->GetFreeItem();

    visualUpdateNeeded_      = false;
    visualUpdateRunning_     = true;
    visualUpdateToBeApplied_ = false;
    cubesphereVisualTh_ = cubesphereVisual_;

    wi->priority_ = 0;
    wi->aux_ = this;
    wi->workFunction_ = workFuncVisual;
    wq->AddWorkItem( wi );
}

void SphereItem::processSubdriveVisual()
{
    cubesphereVisualTh_.subdrive( &subdriveSourceVisual_ );
    applySourceVisual( cubesphereVisualTh_ );

    trianglesVisual_.Clear();
    cubesphereVisualTh_.triangleList( trianglesVisual_ );

    visualUpdateToBeApplied_ = true;
    visualUpdateRunning_ = false;
}

void SphereItem::finishSubdriveVisual()
{
    if ( !visualUpdateToBeApplied_ )
        return;
    visualUpdateToBeApplied_ = false;

    cubesphereVisual_ = cubesphereVisualTh_;

    CustomGeometry * cg = geometry_;

    cg->Clear();
    cg->SetNumGeometries( 1 );
    cg->BeginGeometry( 0, TRIANGLE_LIST );

    const unsigned qty = trianglesVisual_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const Vertex & v = trianglesVisual_[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );
        const Color & c( v.color );
        cg->DefineVertex( at );
        cg->DefineColor( c );
        cg->DefineNormal( -n );
    }

    cg->Commit();

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    Material * m = cache->GetResource<Material>( material_ );
    //Material * m = cache->GetResource<Material>("Materials/Stone.xml");
    //m->SetFillMode( FILL_WIREFRAME );
    cg->SetMaterial( m );
    cg->SetCastShadows( true );
    cg->SetDynamic( true );
}


void SphereItem::regenerateMeshVisual()
{
    trianglesVisual_.Clear();
    cubesphereVisual_.triangleList( trianglesVisual_ );


    CustomGeometry * cg = geometry_;

    cg->Clear();
    cg->SetNumGeometries( 1 );
    cg->BeginGeometry( 0, TRIANGLE_LIST );

    const unsigned qty = trianglesVisual_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const Vertex & v = trianglesVisual_[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );
        const Color & c( v.color );
        cg->DefineVertex( at );
        cg->DefineColor( c );
        cg->DefineNormal( -n );
    }

    cg->Commit();

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    Material * m = cache->GetResource<Material>( material_ );
    //Material * m = cache->GetResource<Material>("Materials/Stone.xml");
    //m->SetFillMode( FILL_WIREFRAME );
    cg->SetMaterial( m );
    cg->SetCastShadows( true );
    cg->SetDynamic( true );
}





static void workFuncCollision( const WorkItem * item, unsigned index )
{
    SphereItem * si = reinterpret_cast<SphereItem *>( item->aux_ );
    if ( !si )
        return;

    si->processSubdriveCollision();
}

static void workFuncVisual( const WorkItem * item, unsigned index )
{
    SphereItem * si = reinterpret_cast<SphereItem *>( item->aux_ );
    if ( !si )
        return;

    si->processSubdriveVisual();
}







}






