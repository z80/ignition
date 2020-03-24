
#ifndef __PHYSICS_CHARACTER_ITEM_H_
#define __PHYSICS_CHARACTER_ITEM_H_

#include "physics_item.h"

using namespace Urho3D;

namespace Ign
{

class PhysicsCharacterItem: public PhysicsItem
{
    URHO3D_OBJECT( PhysicsCharacterItem, PhysicsItem )
public:
    static void RegisterComponent( Context * context );

    PhysicsCharacterItem( Context * context );
    virtual ~PhysicsCharacterItem();

    void setAzimuth( Float az );
    Float azimuth() const;
protected:
    virtual void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;
    virtual void physicsUpdate( RigidBody2 * rb ) override;
    virtual void orientRigidBody(RigidBody2 *rb );

    Float azimuth_;
};


}





#endif




