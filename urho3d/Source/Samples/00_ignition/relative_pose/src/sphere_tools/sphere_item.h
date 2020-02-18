
#ifndef __SPHERE_ITEM_H_
#define __SPHERE_ITEM_H_

#include "ref_frame.h"
#include "cube_sphere.h"

using namespace Urho3D;

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
    void updateCollisionData();
    void updateVisualData();

    void refStateChanged() override;
    void poseChanged() override;

    /// Level of detail subdivision initialization.
    virtual void subdriveLevelsInit();

    // Processing physics frame position changes
    //void childEntered( RefFrame * refFrame ) override;
    //void childLeft( RefFrame * refFrame ) override;
    //void parentTeleported() override;
    //void childTeleported( RefFrame * refFrame ) override;

protected:
    /// Handle scene being assigned. This may happen several times
    /// during the component's lifetime. Scene-wide subsystems and events
    /// are subscribed to here.
    virtual void OnSceneSet( Scene * scene );

    /// Apply source to a subdivided sphere.
    /// Need to override this one in an implementation!!!
    virtual void applySourceCollision( Cubesphere & cs );
    /// Apply source to a subdivided sphere.
    /// /// Need to override this one in an implementation!!!
    virtual void applySourceVisual( Cubesphere & cs );

    /// Check all children and subdivide based on where 
    /// physics frames are located. This is done on a server side.
    void subdivideCollision();
    /// Subdivides for visualization. This is done on client side.
    void subdivideVisual();
    /// Rebuilds CustomGeometry thing based on 
    /// triangles obtained from "cubesphereVisual_".
    void regenerateMeshVisual();
public:
    Vector<Vector3d> pts_;
    Cubesphere     cubesphereCollision_, 
                   cubesphereVisual_;
    SubdriveSource subdriveSourceCollision_,
                   subdriveSourceVisual_;

    // These two are for client side.
    SharedPtr<Node> node_;
    SharedPtr<CustomGeometry> geometry_;

    // For buffering visual geometry.
    Vector<Vertex> trianglesVisual_;

};


}



#endif







