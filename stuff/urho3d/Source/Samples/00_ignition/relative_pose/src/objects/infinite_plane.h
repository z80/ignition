
#ifndef __INFINITE_PLANE_H_
#define __INFINITE_PLANE_H_

#include "Urho3D/Urho3DAll.h"
#include "physics_item.h"

namespace Ign
{

class InfinitePlane: public PhysicsItem
{
    URHO3D_OBJECT( InfinitePlane, PhysicsItem )
public:
    static void RegisterComponent( Context * context );

    InfinitePlane( Context * context );
    ~InfinitePlane();

    Float distance( unsigned refFrameId ) override;
    Float distance( const Vector3d & r=Vector3d::ZERO ) const override;
    void refStateChanged() override;
    void parentTeleported() override;

protected:
    void createVisualContent( Node * n ) override;
    void setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs ) override;

private:
    Vector<SharedPtr<Node> > nodes_;
    static const int QTY;
    static const Float STEP;
};

}


#endif





