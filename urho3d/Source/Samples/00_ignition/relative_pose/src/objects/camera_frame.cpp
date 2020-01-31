
#include "camera_frame.h"
#include "environment.h"
#include "physics_frame.h"
#include "physics_item.h"
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

CameraFrame::CameraFrame( Context * context )
    : RefFrame( context )
{
    yaw_   = 30.0 / 180.0 * 3.14;
    pitch_ = 45.0 / 180.0 * 3.14;
    dist_  = 8.0;
}

CameraFrame::~CameraFrame()
{

}

void CameraFrame::ApplyControls( const Controls & ctrl )
{
    yaw_   = ctrl.yaw_ * 180.0 / 3.14 / 10.0;
    pitch_ = ctrl.pitch_ * 180.0 / 3.14 / 10.0;
    VariantMap::ConstIterator it = ctrl.extraData_.Find( IGN_ZOOM_VALUE );
    const int z = (it != ctrl.extraData_.End()) ? it->second_.GetInt() : 5;
    dist_ = static_cast<Float>( z ) * alpha_;

    RefFrame * directParent = parent();
    RefFrame * originParent = ( directParent ) ? directParent->parent() : nullptr;
    Quaterniond q;
    q.FromEulerAngles( pitch_, yaw_, 0.0 );
    if ( directParent && originParent )
    {
        Vector3d    rel_r;
        Quaterniond rel_q;
        originParent->relativePose( directParent, rel_r, rel_q );
        q = rel_q * q;
    }
    Vector3d r( 0.0, 0.0, -1.0 );
    r = q * r;
    r *= dist_;

    setR( r );
    setQ( q );
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
    Node * n = c->GetNode();
    assignRefFrame( n );
    node_ = SharedPtr<Node>( n );

    {
        const String stri = "Assigned CameraFrame userId " + String( id );
        Notifications::AddNotification( GetContext(), stri ); 
    }
}

void CameraFrame::OnSceneSet( Scene * scene )
{
    assignCameraNode();
}


}


