

#ifndef __SURFACE_COLLISION_MESH_H_
#define __SURFACE_COLLISION_MESH_H_

#include "physics_item.h"

using namespace Urho3D;

namespace Ign
{

class SurfaceCollisionMesh: public PhysicsItem
{
    URHO3D_OBJECT( SurfaceCollsionMesh, PhysicsItem )
public:
    static void RegisterComponent( Context * context );

    SurfaceCollsionMesh( Context * context );
    ~SurfaceCollsionMesh();

    bool IsSelectable() const override;

protected:
    /// Creation of visual content when scene is set.
    void createVisualContent( Node * n ) override;
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;
};


}









#endif







