

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

    /// Phsyics frame should not integrate dynamics if unless this returns true.
    bool valid();

protected:
    /// Callback for SphereItem done its physical surface update.
    void OnSphereItemUpdate( StringHash eventId, VariantMap & eventData );

    void OnSceneSet( Scene * scene ) override;
    /// Creation of visual content when scene is set.
    void createVisualContent( Node * n ) override;
    /// Set physical content parameters (mass, inertia tensor, collision shape, etc.).
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;

public:
    /// Go to parent of parent and search for SphereItem within its children.
    SphereItem * pickSphere();
    bool needRebuild(SphereItem * & item, bool forceRebuild=false );
    void constructCustomGeometry( bool forceRebuild=false );
    void constructCustomGeometry( SphereItem * si, CustomGeometry * cg );

protected:
    State lastState_;
    SphereItem * lastSphereItem_;

    SharedPtr<CustomGeometry> customGeometry_,
                              visualCustomGeometry_;
    Vector<SubdriveSource::SubdividePoint> pts_;
    Vector<Vertex>   tris_;

    //Timer timer_;
};


}









#endif







