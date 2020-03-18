
#ifndef __PHYSICS_FRAME_H_
#define __PHYSICS_FRAME_H_

#include "Urho3D/Urho3DAll.h"
#include "ref_frame.h"
#include "physics_world_2.h"
#include "environment.h"

using namespace Urho3D;

namespace Ign
{

class PhysicsFrame: public RefFrame
{
    URHO3D_OBJECT( PhysicsFrame, RefFrame )
public:
    /// Register object factory.
    static void RegisterComponent( Context * context );

    PhysicsFrame( Context * context );
    ~PhysicsFrame();

    virtual void DrawDebugGeometry( DebugRenderer * debug, bool depthTest ) override;

    void physicsStep( float sec_dt );
    bool handleSplitMerge();
    Node * physicsNode();

protected:
    /// Handle scene being assigned. This may happen several times
    /// during the component's lifetime. Scene-wide subsystems and events
    /// are subscribed to here.
    void OnSceneSet( Scene * scene ) override;

    /// Apply forces from parents of this ref. frame.
    void applyForces();

    /// Update all physics item states based on dynamics simulation.
    void updateChildStates();

    const Vector<SharedPtr<RefFrame> > & userControlledObjects();
    /// After dynamics step check all user controlled objects and determine
    /// if it is necessary to exclude objects from simulation.
    void checkInnerObjects();
    /// Check external objects if it is necessary to include some into
    /// the simulation.
    void checkOuterObjects();
    /// Check if it's worth to exists.
    /// If there are no user controlled objects left parent all inner objects
    /// to the parent of this node and destroy itself.
    bool checkIfWorthToExist();
    /// Check if it's worth to teleport.
    void checkIfTeleport();
    /// Check if need split into a few physics nodes.
    bool checkIfNeedToSplit();
    /// Check if need to merge with another physics frame.
    bool checkIfNeedToMerge();
public:
    SharedPtr<Node>          node_;
    SharedPtr<PhysicsWorld2> physicsWorld_;

    // This is temporary holder.
    Vector<SharedPtr<RefFrame> > userControlledList_, userControlledList2_;
};


}


#endif



