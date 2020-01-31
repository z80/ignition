

#ifndef __SURFACE_COLLISION_MESH_H_
#define __SURFACE_COLLISION_MESH_H_

#include "physics_item.h"

using namespace Urho3D;

namespace Ign
{

class SphereItem;

class SurfaceCollisionMesh: public PhysicsItem
{
    URHO3D_OBJECT( SurfaceCollisionMesh, PhysicsItem )
public:
    static void RegisterComponent( Context * context );

    SurfaceCollisionMesh( Context * context );
    ~SurfaceCollisionMesh();

    void parentTeleported() override;

    bool IsSelectable() const override;

protected:
    /// Creation of visual content when scene is set.
    void createVisualContent( Node * n ) override;
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;

    /// Go to parent of parent and search for SphereItem within its children.
    SphereItem * pickSphere();
};


}









#endif







