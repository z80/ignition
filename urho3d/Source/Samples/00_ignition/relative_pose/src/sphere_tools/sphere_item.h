
#ifndef __SPHERE_ITEM_H_
#define __SPHERE_ITEM_H_

#include "ref_frame.h"
#include "cube_sphere.h"

namespace Ign
{

class SphereItem: public RefFrame
{
    URHO3D_OBJECT( SphereItem, RefFrame )
public:
    /// Register object factory.
    static void RegisterComponent( Context * context );

    SphereItem( Context * context );
    virtual ~SphereItem();

    virtual void DrawDebugGeometry( DebugRenderer * debug, bool depthTest ) override;

    /// Update visual appearance and collision subdivision.
    virtual void updateData();


    // Processing physics frame position changes
    void childEntered( RefFrame * refFrame ) override;
    void childLeft( RefFrame * refFrame ) override;
    void parentTeleported() override;
    void childTeleported( RefFrame * refFrame ) override;

protected:
    /// Handle scene being assigned. This may happen several times
    /// during the component's lifetime. Scene-wide subsystems and events
    /// are subscribed to here.
    virtual void OnSceneSet( Scene * scene );

    /// Check all children and subdivide based on where 
    /// physics frames are located.
    void subdivideCollisions();
public:
    Cubesphere cubesphere_;

    // These two are for client side.
    SharedPtr<Node> node_;
    SharedPtr<CustomGeometry> geometry_;
};


}



#endif







