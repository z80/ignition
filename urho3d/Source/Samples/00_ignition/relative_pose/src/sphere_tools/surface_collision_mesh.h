

#ifndef __SURFACE_COLLISION_MESH_H_
#define __SURFACE_COLLISION_MESH_H_

#include "physics_item.h"
#include "cube_sphere.h"

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
    void OnSceneSet( Scene * scene ) override;
    /// Creation of visual content when scene is set.
    void createVisualContent( Node * n ) override;
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;

    /// Go to parent of parent and search for SphereItem within its children.
    SphereItem * pickSphere();
    bool needRebuild( SphereItem * & item );
    void constructCustomGeometry();

    State lastState_;
    SphereItem * lastSphereItem_;

    SharedPtr<CustomGeometry> customGeometry_;
    Vector<Vector3d> pts_;
    Vector<Vertex>   tris_;
};


}









#endif







