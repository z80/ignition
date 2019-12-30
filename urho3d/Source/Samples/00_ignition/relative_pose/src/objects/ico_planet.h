
#ifndef __ICO_PLANET_H_
#define __ICO_PLANET_H_

#include "physics_item.h"

namespace Ign
{

class IcoPlanet: public PhysicsItem
{
    URHO3D_OBJECT( IcoPlanet, PhysicsItem )
public:
    static void RegisterComponent( Context * context );

    IcoPlanet( Context * context );
    ~IcoPlanet();

    void refStateChanged() override;

protected:
    void createVisualContent( Node * n ) override;
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;
};

}



#endif



