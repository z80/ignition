
#include "camera_frame.h"
#include "environment.h"
#include "physics_frame.h"
#include "physics_item.h"

#include "Notifications.h"

namespace Ign
{

void CameraFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<CameraFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );

    URHO3D_ATTRIBUTE( "UserId", int, userId_, -1, AM_DEFAULT );

}

CameraFrame::CameraFrame( Context * context )
    : RefFrame( context )
{

}

CameraFrame::~CameraFrame()
{

}

void CameraFrame::SetUserId( unsigned id )
{
    userId_ = id;
    
    MarkNetworkUpdate();
}

void CameraFrame::UpdatePose( Float sec_dt )
{
    // If parent is physics item take another parent to get to physics frame.
    // If it is not physics item use it as an origin for ref state.
    RefFrame * p = parent();
    if ( !p )
    {
        computeRefState();
        return;
    }
    // Try convert to physicsBody
    PhysicsItem * pi = p->Cast<PhysicsItem>();
    if ( !pi )
    {
        computeRefState();
        return;
    }
    RefFrame * p2 = pi->parent();
    PhysicsFrame * pf = p2->Cast<PhysicsFrame>();
    if ( pf )
    {
        computeRefState( pf );
        return;
    }

    computeRefState( p );
}

void CameraFrame::refStateChanged()
{
    if ( !node_ )
        return;
    const State s = refState();

    const Vector3 r( s.r.x_, s.r.y_, s.r.z_ );
    const Quaternion q( s.q.w_, s.q.x_, s.q.y_, s.q.z_ );

    node->SetPosition( r );
    node->SetRotation( q );
}


void CameraFrame::assignCameraNode()
{
    if ( userId_ < 0 )
      return;

    Scene * s = GetScene();
    if ( !s )
      return;

    Environment * e = s->GetComponent<Environment>();
    if ( !e )
      return;

    // Check if userId matches environment userId and query node only in this
    // case.
    if ( (!e->IsServer()) && (!e->IsCLient()) )
      return;

    if (e->userId_ != userId_)
      return;

    Camera * c = s->GetComponent<Camera>( true );
    Node * n = c->GetNode();
    assignRefFrame( n );
    node_ = SharedPtr<Node>( n );

    {
        const String stri = "Assigned CameraFrame userId " + String( userId_ );
        Notifications::AddNotification( GetContext(), stri ); 
    }
}

void CameraFrame::OnSceneSet( Scene * scene )
{
    assignCameraNode();
}


}


