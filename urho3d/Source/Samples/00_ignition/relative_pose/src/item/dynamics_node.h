
#ifndef __DYNAMICS_NODE_H_
#define __DYNAMICS_NODE_H_

#include "Urho3D/Urho3DAll.h"
#include "ref_frame.h"
#include "physics_world_2.h"
#include "environment.h"

using namespace Urho3D;

namespace Ign
{

class DynamicsNode: public RefFrame
{
    URHO3D_OBJECT( DynamicsNode, RefFrame )
public:
    DynamicsNode( Context * context );
    ~DynamicsNode();

    void dynamicsStep( float sec_dt );

    virtual void childEntered( RefFrame * refFrame );
    virtual void childLeft( RefFrame * refFrame );

protected:
    /// Handle scene being assigned. This may happen several times
    /// during the component's lifetime. Scene-wide subsystems and events
    /// are subscribed to here.
    virtual void OnSceneSet( Scene * scene );

    /// After dynamics step check all user controlled objects and determine
    /// if it is necessary to exclude objects from simulation.
    void checkInnerObjects();
    /// Check external objects if it is necessary to include some into
    /// the simulation.
    void checkOuterObjects();
    /// Check if it's worth to exists.
    /// If there are no user controlled objects left parent all inner objects
    /// to the parent of this node and destroy itself.
    void checkIfWorthToExist();
    /// Check if it's worth to teleport.
    void checkIfTeleport( const Vector<SharedPtr<RefFrame>> & objs );

public:
    SharedPtr<Environment>   env_;
    SharedPtr<Node>          node_;
    SharedPtr<PhysicsWorld2> physicsWorld_;
};


}


#endif



