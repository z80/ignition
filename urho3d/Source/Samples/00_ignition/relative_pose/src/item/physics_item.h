
#ifndef __PHYSICS_ITEM_H_
#define __PHYSICS_ITEM_H_

#include "Urho3D/Urho3DAll.h"
#include "ref_frame.h"
#include "rigid_body_2.h"
#include "collision_shape_2.h"
#include "environment.h"

using namespace Urho3D;

namespace Ign
{


class PhysicsItem: public RefFrame
{
    URHO3D_OBJECT( PhysicsItem, RefFrame )
public:
    /// Register object factory.
    static void RegisterObject( Context * context );

    PhysicsItem( Context * context );
    virtual ~PhysicsItem();

    /// This one is called by PhysicsFrame instance this item is in 
    /// after physics update step. 
    void updateStateFromRigidBody();
protected:
    /// Called when this thing is moved into another ref. frame.
    void enteredRefFrame( RefFrame * refFrame ) override;
    /// Called when this thing is moved out of it's current parent.
    void leftRefFrame( RefFrame * refFrame ) override;

    /// When user controlled flag changes call this method.
    /// Called just before changing appropriate field so there
    /// is a way to know what it was before.
    void userControlledChanged( bool newUserControlled ) override;


    /// Handle scene being assigned. This may happen several times
    /// during the component's lifetime. Scene-wide subsystems and events
    /// are subscribed to here.
    void OnSceneSet( Scene * scene ) override;

    /// Creation of visual content when scene is set.
    virtual void createVisualContent( Node * n );
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    virtual void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs );


    /// On server side it has a 
    /// 1) node 
    /// 2) rigid body
    /// 3) collision shape
    /// On client side it is supposed to have 
    /// 1) node 
    /// 2) visual representation. At least mesh or any other type of 
    ///    visual representation.

    // Server side objects.
    SharedPtr<Node>            physics_node_;
    SharedPtr<RigidBody2>      rigid_body_;
    SharedPtr<CollisionShape2> collision_shape_;

    // Client side objects.
    // Content of visual node is supposed to depend on 
    // how far the object is from local client controlled node.
    // For example if it is extremely far there is no need in showing any 
    // type of visual and it just can be disabled.
    SharedPtr<Node>            visual_node_;
};


}



#endif




