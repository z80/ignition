
#ifndef __PHYSICS_CHARACTER_ITEM_H_
#define __PHYSICS_CHARACTER_ITEM_H_

#include "physics_item.h"

using namespace Urho3D;

namespace Ign
{

class OrbitingFrame;

class PhysicsCharacterItem: public PhysicsItem
{
    URHO3D_OBJECT( PhysicsCharacterItem, PhysicsItem )
public:
    static void RegisterComponent( Context * context );

    PhysicsCharacterItem( Context * context );
    virtual ~PhysicsCharacterItem();

    void setAzimuth( Float az );
    Float azimuth() const;

    void refStateChanged() override;
    virtual void enteredRefFrame( unsigned refFrameId ) override;

protected:
    virtual void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;
    virtual void physicsUpdate( RigidBody2 * rb ) override;
    virtual void orientRigidBody(RigidBody2 *rb );

    void initGeocentric();
    void adjustGeocentric();
    static OrbitingFrame * orbitingFrame( RefFrame * rf );

    bool geocentric_initialized_;
    Float azimuth_;
    // Previous "up" vector.
    Vector3d geocentric_last_up_;
    Quaterniond surfQ_;

};


}





#endif




