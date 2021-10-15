
#ifndef __DYNAMIC_CUBE_H_
#define __DYNAMIC_CUBE_H_

#include "physics_item.h"

using namespace Urho3D;

namespace Ign
{

class DynamicCube: public PhysicsItem
{
    URHO3D_OBJECT( DynamicCube, PhysicsItem )
public:
    static void RegisterComponent( Context * context );

    DynamicCube( Context * context );
    ~DynamicCube();

    void ApplyControls( const Controls & ctrl, Float dt ) override;
    bool IsSelectable() const override;
    void Trigger( const VariantMap & data ) override;

protected:
    /// Creation of visual content when scene is set.
    void createVisualContent( Node * n ) override;
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;

    bool thrustEnabled_;
};


}




#endif



