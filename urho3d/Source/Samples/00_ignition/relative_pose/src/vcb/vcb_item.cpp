
#include "vcb_item.h"
#include "environment.h"

namespace Ign
{

// Server sends to appropriate client when use controlled object enters 
// into proximity of VCB. Client should show appropriate GUI.
URHO3D_EVENT( E_VCB_CLIENT_ENTERED, VcbClientEntered )
{
    URHO3D_PARAM( P_CLIENT_ID, Id ); // string - level object name
}

// Client should hide all related GUIs.
URHO3D_EVENT( E_VCB_CLIENT_LEFT, VcbClientLeft )
{
    URHO3D_PARAM( P_CLIENT_ID, Id ); // string - level object name
}

// Client sends an event if he wants to enter build mode.
URHO3D_EVENT( E_VCB_ENTER_BUILD_MODE, VcbEnterBuildMode )
{
    URHO3D_PARAM( P_CLIENT_ID, Id ); // string - level object name
}

// Client sends the event when he wants to return to its regular life.
URHO3D_EVENT( E_VCB_LEAVE_BUILD_MODE, VcbLeaveBuildMode )
{
    URHO3D_PARAM( P_CLIENT_ID, Id ); // string - level object name
}


void VcbItem::RegisterComponent( Context * context )
{
    context->RegisterFactory<VcbItem>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );

    Network * n = context->GetSubsystem<Network>();
    n->RegisterRemoteEvent( E_VCB_CLIENT_ENTERED );
    n->RegisterRemoteEvent( E_VCB_CLIENT_LEFT );
    n->RegisterRemoteEvent( E_VCB_ENTER_BUILD_MODE );
    n->RegisterRemoteEvent( E_VCB_LEAVE_BUILD_MODE );
}

VcbItem::VcbItem( Context * context )
    : PhysicsItem( context )
{
}

VcbItem::~VcbItem()
{
}

void VcbItem::HandleClientEntered_Remote( StringHash eventType, VariantMap & eventData )
{
    using namespace VcbClientEntered;
    const int clientId = eventData[P_CLIENT_ID].GetInt();
    Environment * e = Environment::environment( context_ );
    if ( !e )
        return;
    const ClientDesc & cd = e->clientDesc();
    if ( cd.id_ != clientId )
        return;

    // Show the Enter GUI.
    if ( !enter_gui_ )
    {
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        XMLFile * f = cache->GetResource<XMLFile>( "Ign/UI/EnterWnd.xml" );
        if ( !f )
            return;
        UI * ui = GetSubsystem<UI>();
        enter_gui_ = ui->LoadLayout( f );
        UIElement * root = ui->GetRoot();

        enter_gui_->SetAlignment( HA_RIGHT, VA_TOP );
        root->AddChild( enter_gui_ );

        Graphics * graphics = GetSubsystem<Graphics>();
        const int w = graphics->GetWidth();
        enter_gui_->SetPosition( w, 0 );

        enter_gui_->SetEnabled( true );
    }
}

void VcbItem::HandleClientLeft_Remote( StringHash eventType, VariantMap & eventData )
{
    using namespace VcbClientLeft;
    const int clientId = eventData[P_CLIENT_ID].GetInt();
    Environment * e = Environment::environment( context_ );
    if ( !e )
        return;
    const ClientDesc & cd = e->clientDesc();
    if ( cd.id_ != clientId )
        return;

    // Hide the all the GUI.
    if ( enter_gui_ )
        enter_gui_->SetEnabled( true );
    if ( leave_gui_ )
        leave_gui_->SetEnabled( true );
}

void VcbItem::HandleEnterBuildMode_Remote( StringHash eventType, VariantMap & eventData )
{
    // Parent camera to VCB.
}

void VcbItem::HandleLeaveBuildMode_Remote( StringHash eventType, VariantMap & eventData )
{
}


void VcbItem::createVisualContent( Node * n )
{
}

void VcbItem::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
}




}

