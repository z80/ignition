
#ifndef __PHYSICS_ITEM_H_
#define __PHYSICS_ITEM_H_

#include "Urho3D/Urho3DAll.h"
#include "ref_frame.h"
#include "physics_world_2.h"
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

protected:
    /// Called when this thing is moved into another ref. frame.
    virtual void enteredRefFrame( RefFrame * refFrame );
    /// Called when this thing is moved out of it's current parent.
    virtual void leftRefFrame( RefFrame * refFrame );
    /// Called when parent teleported.
    virtual void parentTeleported();
    /// Called when child teleported.
    virtual void childTeleported( RefFrame * refFrame );

    /// When user controlled flag changes call this method.
    /// Called just before changing appropriate field so there
    /// is a way to know what it was before.
    virtual void userControlledChanged( bool newUserControlled );


    /// Handle scene being assigned. This may happen several times
    /// during the component's lifetime. Scene-wide subsystems and events
    /// are subscribed to here.
    virtual void OnSceneSet( Scene * scene );


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




