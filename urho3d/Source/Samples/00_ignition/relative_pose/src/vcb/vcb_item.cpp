
#include "vcb_item.h"
#include "environment.h"
#include "camera_frame.h"
#include "settings.h"

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
    setName( "VcbItem" );

    SubscribeToRemoteEvents();
}

VcbItem::~VcbItem()
{
}

void VcbItem::UpdateClient_ServerSide( Connection * c, const ClientDesc & cd, RefFrame * selectedObj )
{
    if ( !selectedObj )
        return;
    // Check if client Id is in the list.
    const int clientId = cd.id_;
    HashMap<int, SharedPtr<RefFrame> >::Iterator it = clients_inside_.Find( clientId );
    if ( it == clients_inside_.End() )
    {
        const Float maxDist = Settings::vcbEnterDistance();
        const Float dist = selectedObj->distance( this );
        if ( dist > maxDist )
            return;
        clients_inside_[clientId] = SharedPtr<RefFrame>( selectedObj );
        VariantMap & eventData = this->GetEventDataMap();
        eventData[VcbClientEntered::P_CLIENT_ID] = clientId;
        if ( c )
            // Call handler remotely.
            c->SendRemoteEvent( E_VCB_CLIENT_ENTERED, true, eventData );
        else
            // Call handler locally.
            HandleClientEntered_Remote( E_VCB_CLIENT_ENTERED, eventData );
    }
    else
    {
        const Float maxDist = Settings::vcbLeaveDistance();
        const Float dist = selectedObj->distance( this );
        if ( dist < maxDist )
            return;
        clients_inside_.Erase( it );
        VariantMap & eventData = this->GetEventDataMap();
        eventData[VcbClientEntered::P_CLIENT_ID] = clientId;
        if ( c )
            // Call handler remotely.
            c->SendRemoteEvent( E_VCB_CLIENT_LEFT, true, eventData );
        else
            // Call handler locally.
            HandleClientLeft_Remote( E_VCB_CLIENT_LEFT, eventData );
    }
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
        XMLFile * f = cache->GetResource<XMLFile>( "Ign/UI/VcbEnter.xml" );
        if ( !f )
            return;
        UI * ui = GetSubsystem<UI>();
        enter_gui_ = ui->LoadLayout( f );
        UIElement * root = ui->GetRoot();

        enter_gui_->SetAlignment( HA_RIGHT, VA_TOP );
        //enter_gui_->SetAlignment( HA_CENTER, VA_CENTER );
        root->AddChild( enter_gui_ );

        Graphics * graphics = GetSubsystem<Graphics>();
        const int w = graphics->GetWidth();
        enter_gui_->SetPosition( -20, 20 );

        SubscribeToEnterGuiEvents();
    }
    enter_gui_->SetVisible( true );
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
        enter_gui_->SetVisible( false );
    if ( leave_gui_ )
        leave_gui_->SetVisible( false );
}

void VcbItem::HandleEnterBuildMode_Remote( StringHash eventType, VariantMap & eventData )
{
    // Parent camera to VCB.
    using namespace VcbEnterBuildMode;
    const int id = eventData[VcbEnterBuildMode::P_CLIENT_ID].GetInt();
    Environment * e = Environment::environment( context_ );
    CameraFrame * cf = e->FindCameraFrame( id );
    if ( !cf )
        return;
    RefFrame * rf = cf->FocusedFrame();
    if ( rf )
    {
        // Remember the object to return camera focus to.
        client_objects_[ cf ] = SharedPtr<RefFrame>( rf );
    }
    cf->Focus( this );

    // Open appropriate GUI.
}

void VcbItem::HandleLeaveBuildMode_Remote( StringHash eventType, VariantMap & eventData )
{
    // Parent camera back.
    using namespace VcbEnterBuildMode;
    const int id = eventData[VcbEnterBuildMode::P_CLIENT_ID].GetInt();
    Environment * e = Environment::environment( context_ );
    CameraFrame * cf = e->FindCameraFrame( id );
    if ( !cf )
        return;
    // Restore the camera focus.
    HashMap<CameraFrame *, SharedPtr<RefFrame> >::Iterator it = client_objects_.Find( cf );
    if ( it != client_objects_.End() )
    {
        RefFrame * rf = it->second_;
        cf->Focus( rf );
    }
    
    // Hide the building GUI.
}

void VcbItem::HandleEnterBuildModeClicked( StringHash eventType, VariantMap & eventData )
{
    Network * n = GetSubsystem<Network>();
    Connection * c = n->GetServerConnection();
    Environment * e = Environment::environment( context_ );
    if ( !e )
        return;
    const ClientDesc & cd = e->clientDesc();

    // Show the Enter GUI.
    if ( !leave_gui_ )
    {
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        XMLFile * f = cache->GetResource<XMLFile>( "Ign/UI/VcbLeave.xml" );
        if ( !f )
            return;
        UI * ui = GetSubsystem<UI>();
        leave_gui_ = ui->LoadLayout( f );
        UIElement * root = ui->GetRoot();

        leave_gui_->SetAlignment( HA_RIGHT, VA_TOP );
        root->AddChild( leave_gui_ );

        Graphics * graphics = GetSubsystem<Graphics>();
        const int w = graphics->GetWidth();
        leave_gui_->SetPosition( -20, 20 );

        SubscribeToLeaveGuiEvents();
    }
    leave_gui_->SetVisible( true );
    if ( enter_gui_ )
        enter_gui_->SetVisible( false );

    VariantMap & eData = this->GetEventDataMap();
    eventData[VcbEnterBuildMode::P_CLIENT_ID] = cd.id_;
    if ( c )
        // Call handler remotely.
        c->SendRemoteEvent( E_VCB_ENTER_BUILD_MODE, true, eData );
    else
        // Call handler locally.
        HandleEnterBuildMode_Remote( E_VCB_ENTER_BUILD_MODE, eData );
}

void VcbItem::HandleLeaveBuildModeClicked( StringHash eventType, VariantMap & eventData )
{
    Network * n = GetSubsystem<Network>();
    Connection * c = n->GetServerConnection();
    Environment * e = Environment::environment( context_ );
    if ( !e )
        return;
    const ClientDesc & cd = e->clientDesc();

    if ( leave_gui_ )
        leave_gui_->SetVisible( false );
    if ( enter_gui_ )
        enter_gui_->SetVisible( true );

    VariantMap & eData = this->GetEventDataMap();
    eventData[VcbLeaveBuildMode::P_CLIENT_ID] = cd.id_;
    if ( c )
        // Call handler remotely.
        c->SendRemoteEvent( E_VCB_LEAVE_BUILD_MODE, true, eData );
    else
        // Call handler locally.
        HandleLeaveBuildMode_Remote( E_VCB_LEAVE_BUILD_MODE, eData );
}

void VcbItem::createVisualContent( Node * n )
{
    if ( !n )
        return;

    ResourceCache * cache = GetSubsystem<ResourceCache>();

    StaticModel * model = n->CreateComponent<StaticModel>( LOCAL );
    model->SetModel( cache->GetResource<Model>("Ign/Models/Vcb.mdl") );
    Material * material = cache->GetResource<Material>("Ign/Materials/Vcb_M.xml");
    model->SetMaterial( material );
}

void VcbItem::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 0.0f );
    cs->SetCylinder( 16.0f, 2.0f, Vector3( 0.0, 0.0f, 0.0f ) );
}

void VcbItem::SubscribeToRemoteEvents()
{
    SubscribeToEvent( E_VCB_CLIENT_ENTERED,   URHO3D_HANDLER( VcbItem,  HandleClientEntered_Remote ) );
    SubscribeToEvent( E_VCB_CLIENT_LEFT,      URHO3D_HANDLER( VcbItem,  HandleClientLeft_Remote ) );
    SubscribeToEvent( E_VCB_ENTER_BUILD_MODE, URHO3D_HANDLER( VcbItem,  HandleEnterBuildMode_Remote ) );
    SubscribeToEvent( E_VCB_LEAVE_BUILD_MODE, URHO3D_HANDLER( VcbItem,  HandleLeaveBuildMode_Remote ) );
}

void VcbItem::SubscribeToEnterGuiEvents()
{
    if ( !enter_gui_ )
        return;
    UIElement * e = enter_gui_->GetChild( "EnterBtn", true );
    if ( !e )
        return;
    Button * btn = e->Cast<Button>();
    SubscribeToEvent( btn, E_RELEASED, URHO3D_HANDLER( VcbItem, HandleEnterBuildModeClicked ) );
}

void VcbItem::SubscribeToLeaveGuiEvents()
{
    if ( !leave_gui_ )
        return;
    UIElement * e = leave_gui_->GetChild( "LeaveBtn", true );
    if ( !e )
        return;
    Button * btn = e->Cast<Button>();
    SubscribeToEvent( btn, E_RELEASED, URHO3D_HANDLER( VcbItem, HandleLeaveBuildModeClicked ) );
}





}

