
#ifndef __SPHERE_ITEM_H_
#define __SPHERE_ITEM_H_

#include "force_source_frame.h"
#include "cube_sphere.h"

using namespace Urho3D;

namespace Ign
{

// Message to indicate Update of collision sphere.
URHO3D_EVENT( E_SPHERE_COLLISION_UPDATED, SphereCollisionUpdated )
{
    // unsigned ID of the object.
    URHO3D_PARAM( P_REF_FRAME_ID, RefFrameId );
}


class SphereItem: public ForceSourceFrame
{
    URHO3D_OBJECT( SphereItem, ForceSourceFrame )
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

    void setMaterialName( const String & material );

    // Processing physics frame position changes
    //void childEntered( RefFrame * refFrame ) override;
    //void childLeft( RefFrame * refFrame ) override;
    //void parentTeleported() override;
    //void childTeleported( RefFrame * refFrame ) override;

    /// Shows if the object has a valid collision surface generated.
    /// It is needed because this process is asynchronous and time consuming.
    /// Will not integrate dynamics until this thing is valid.
    bool valid() const;

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
    /// This one is synchronous.
    void subdivideCollision();
public:
    void checkIfSubdriveCollisionNeeded();
    void startSubdriveCollision();
    void processSubdriveCollision();
    void finishSubdriveCollision();
protected:
    /// Subdivides for visualization. This is done on client side.
    /// This one is synchonous.
    void subdivideVisual();
public:
    void checkIfSubdriveVisualNeeded();
    void startSubdriveVisual();
    void processSubdriveVisual();
    void finishSubdriveVisual();
protected:
    /// Rebuilds CustomGeometry thing based on 
    /// triangles obtained from "cubesphereVisual_".
    void regenerateMeshVisual();
public:
    String           material_;
    Vector<SubdriveSource::SubdividePoint> ptsCollision_,  ptsVisual_;
    Cubesphere     cubesphereCollision_, cubesphereCollisionTh_, 
                   cubesphereVisual_, cubesphereVisualTh_;
    SubdriveSource subdriveSourceCollision_,
                   subdriveSourceVisual_;

    // These two are for client side.
    SharedPtr<Node> node_;
    SharedPtr<CustomGeometry> geometry_;

    // For buffering visual geometry.
    Vector<Vertex> trianglesVisual_;

    /// For asynchonous implementation.
    volatile bool       visualUpdateNeeded_,    visualUpdateRunning_,    visualUpdateToBeApplied_, 
                        collisionUpdateNeeded_, collisionUpdateRunning_, collisionUpdateToBeApplied_;
    /// For holding the state.
    bool valid_;
};


}



#endif







