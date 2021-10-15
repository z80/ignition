
#ifndef __VCB_ITEM_H_
#define __VCB_ITEM_H_

#include "physics_item.h"

namespace Ign
{

class VcbItem: public PhysicsItem
{
    URHO3D_OBJECT( VcbItem, PhysicsItem )
public:
    /// Register object factory.
    static void RegisterComponent( Context * context );

    VcbItem( Context * context );
    ~VcbItem();

    /// Server part of client update cycle.
    void UpdateClient_ServerSide( Connection * c, const ClientDesc & cd, RefFrame * selectedObj );

    void HandleClientEntered_Remote( StringHash eventType, VariantMap & eventData );
    void HandleClientLeft_Remote( StringHash eventType, VariantMap & eventData );
    void HandleEnterBuildMode_Remote( StringHash eventType, VariantMap & eventData );
    void HandleLeaveBuildMode_Remote( StringHash eventType, VariantMap & eventData );
    /// GUI button responses.
    void HandleEnterBuildModeClicked( StringHash eventType, VariantMap & eventData );
    void HandleLeaveBuildModeClicked( StringHash eventType, VariantMap & eventData );
protected:
    /// Creation of visual content when scene is set.
    void createVisualContent( Node * n ) override;
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;

    /// Sybscribe to remote events
    void SubscribeToRemoteEvents();
    void SubscribeToEnterGuiEvents();
    void SubscribeToLeaveGuiEvents();

    /// Client side enter/leave GUI.
    /// .....
    SharedPtr<UIElement> enter_gui_;
    SharedPtr<UIElement> leave_gui_;
    /// Client side construction GUI.
    /// .....
    SharedPtr<UIElement> main_menu_;

    /// List of clients inside the sensitivity radius.
    HashMap<int, SharedPtr<RefFrame> > clients_inside_;
    /// Client objects to return focus to when client leaves.
    HashMap< CameraFrame *, SharedPtr<RefFrame> > client_objects_;
};


}




#endif



