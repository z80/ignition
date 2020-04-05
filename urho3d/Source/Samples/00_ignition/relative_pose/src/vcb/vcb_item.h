
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
    virtual void createVisualContent( Node * n );
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    virtual void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs );

    /// Client side enter/leave GUI.
    /// .....
    SharedPtr<UIElement> enter_gui_;
    SharedPtr<UIElement> leave_gui_;
    /// Client side construction GUI.
    /// .....

    /// List of clients inside.
    HashMap<int, SharedPtr<RefFrame> > clients_inside_;
};


}




#endif



