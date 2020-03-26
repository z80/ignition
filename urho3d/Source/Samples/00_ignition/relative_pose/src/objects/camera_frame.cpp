
#include "camera_frame.h"
#include "environment.h"
#include "physics_frame.h"
#include "physics_item.h"
#include "orbiting_frame.h"
#include "settings.h"

#include "Notifications.h"
#include "Global3dparty.h"

namespace Ign
{

const Float CameraFrame::alpha_ = 1.2;

void CameraFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<CameraFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}

void CameraFrame::CheckAttributes()
{
    const Vector<AttributeInfo> & attrs = *GetAttributes();
    const unsigned qty = attrs.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const AttributeInfo & ai = attrs[i];
        const String name = ai.name_;
        const VariantType tp = ai.type_;
        const void * ptr = ai.ptr_;
    }
}

CameraFrame::CameraFrame( Context * context )
    : RefFrame( context )
{
    yaw_   = 30.0 / 180.0 * 3.14;
    pitch_ = 45.0 / 180.0 * 3.14;
    dist_  = 28.0;
    //centerBtnPrev_ = false;
    setName( "CameraFrame" );

    camera_mode_ = TGeocentric;
    geocentric_initialized_ = false;
}

CameraFrame::~CameraFrame()
{

}

void CameraFrame::ApplyControls( const Controls & ctrl, Float dt )
{
    yaw_   = ctrl.yaw_ * 180.0 / 3.14 / 10.0;
    pitch_ = ctrl.pitch_ * 180.0 / 3.14 / 10.0;
    // Update distance if it is in the controls.
    VariantMap::ConstIterator it = ctrl.extraData_.Find( IGN_ZOOM_VALUE );
    if ( it != ctrl.extraData_.End() )
    {
        const int z = it->second_.GetInt();
        dist_ = static_cast<Float>( z );
    }

    //RefFrame * directParent = parent();
    //RefFrame * originParent = CameraOrigin();
    Quaterniond q;
    q.FromEulerAngles( pitch_, yaw_, 0.0 );
    /*if ( directParent )
    {
        Vector3d    rel_r;
        Quaterniond rel_q;
        originParent->relativePose( directParent, rel_r, rel_q );
        rel_q = rel_q.Inverse();
        q = rel_q * q;
    }*/
    if ( camera_mode_ == TGeocentric )
    {
        if ( !geocentric_initialized_ )
            initGeocentric();
        else
            adjustGeocentric();
        q = surfQ_ * q;
    }
    else
        geocentric_initialized_ = false;
    Vector3d r( 0.0, 0.0, -1.0 );
    r = q * r;
    r *= dist_;


    setR( r );
    setQ( q );
}

void CameraFrame::setCameraMode( CameraMode mode )
{
    camera_mode_ = mode;
}

RefFrame * CameraFrame::CameraOrigin()
{
    // If parent is physics item take another parent to get to physics frame.
    // If it is not physics item use it as an origin for ref state.
    RefFrame * p = parent();
    if ( !p )
    {
        computeRefState();
        return nullptr;
    }
    // Try convert to physicsBody
    PhysicsItem * pi = p->Cast<PhysicsItem>();
    if ( !pi )
    {
        computeRefState( p );
        return p;
    }
    RefFrame * p2 = pi->parent();
    if ( !p2 )
    {
        computeRefState( p );
        return p;
    }
    PhysicsFrame * pf = p2->Cast<PhysicsFrame>();
    if ( pf )
    {
        computeRefState( pf );
        return pf;
    }

    computeRefState( p );
    return p;
}

void CameraFrame::refStateChanged()
{
    if ( !node_ )
    {
        assignCameraNode();
        if ( !node_ )
            return;
    }
    const State s = refState();

    const Vector3 r( s.r.x_, s.r.y_, s.r.z_ );
    const Quaternion q( s.q.w_, s.q.x_, s.q.y_, s.q.z_ );

    node_->SetPosition( r );
    node_->SetRotation( q );
}


void CameraFrame::assignCameraNode()
{
    const int id = CreatedBy();

    if ( id < 0 )
    {
        node_.Reset();
        return;
    }

    Scene * s = GetScene();
    if ( !s )
    {
        node_.Reset();
        return;
    }

    Environment * e = this->env();
    if ( !e )
        return;

    // Check if userId matches environment userId and query node only in this
    // case.
    if ( (!e->IsServer()) && (!e->IsClient()) )
        return;

    if ( e->clientDesc().id_ != id )
        return;

    Camera * c = s->GetComponent<Camera>( true );
    //c->SetFillMode( FILL_WIREFRAME );
    Node * n = c->GetNode();
    assignRefFrame( n );
    node_ = SharedPtr<Node>( n );

    const Float sz = Settings::cameraMaxDistance() * 1.2;
    c->SetNearClip( Settings::cameraMinDistance() );
    c->SetFarClip( sz );
    Zone * z = s->GetComponent<Zone>();
    z->SetBoundingBox( BoundingBox( -sz, sz ) );
    z->SetFogEnd( sz );

    {
        const String stri = "Assigned CameraFrame userId " + String( id );
        Notifications::AddNotification( GetContext(), stri ); 
    }
}

void CameraFrame::OnSceneSet( Scene * scene )
{
    assignCameraNode();
}

void CameraFrame::initGeocentric()
{
    RefFrame * rf = parent();
    if ( !rf )
        return;
    RefFrame * of = orbitingFrame( rf );
    if ( !of )
        return;
    State rs;
    of->relativeState( rf, rs, true );
    const Quaterniond unrotateParentQ = Quaterniond::IDENTITY; //rf->relQ().Inverse();
    const Vector3d fromG = unrotateParentQ * Vector3d( 0.0, 1.0, 0.0 );
    const Vector3d toG = -(unrotateParentQ * rs.r.Normalized());
    surfQ_.FromRotationTo( fromG, toG );
    geocentric_last_up_ = toG;
    geocentric_initialized_ = true;
}

void CameraFrame::adjustGeocentric()
{
    RefFrame * rf = parent();
    if ( !rf )
        return;
    RefFrame * of = orbitingFrame( rf );
    if ( !of )
        return;
    State rs;
    of->relativeState( rf, rs, true );
    const Quaterniond unrotateParentQ = Quaterniond::IDENTITY; //rf->relQ().Inverse();
    const Vector3d toG = -(unrotateParentQ * rs.r.Normalized());
    const Vector3d fromG = geocentric_last_up_;
    Quaterniond    adjQ;
    adjQ.FromRotationTo( fromG, toG );
    surfQ_ = surfQ_ * adjQ;
    surfQ_.Normalize();
    geocentric_last_up_ = toG;
}

RefFrame * CameraFrame::orbitingFrame( RefFrame * rf )
{
    if ( !rf )
        return nullptr;
    RefFrame * of = rf->Cast<OrbitingFrame>();
    if ( of )
        return of;
    RefFrame * p = rf->parent();
    of = orbitingFrame( p );
    return of;
}



}


