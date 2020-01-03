
#ifndef __FORCE_SOURCE_FRAME_H_
#define __FORCE_SOURCE_FRAME_H_

#include "ref_frame.h"

using namespace Urho3D;

namespace Ign
{

/// Actually, thi class is a subclass of the one described here.
/// It is because force receiver can be a force source for its own 
/// children.
//class ForceReceiverFrame;
class PhysicsItem;

/// It is supposed to be a source of centripital, Coriolis and 
/// gravitational forces. It also might be a source of atmosphere 
/// dynamic friction forces, hydrawlic pressure, etc.
/// It may be recursive. For example, rotating ref. frame 
/// produces centripital and Coriolis forces. And its parent 
/// may produce gravitational force. On the other hand, 
/// orbiting ref. frame must block all parent forces

class ForceSourceFrame: public RefFrame
{
    URHO3D_OBJECT( ForceSourceFrame, RefFrame )
public:
    static void RegisterObject( Context * context );

    ForceSourceFrame( Context * context );
    virtual ~ForceSourceFrame();


    virtual bool Recursive() const;
    virtual bool ProducesForces() const;
    void ApplyForces( PhysicsItem * receiver ) const;
    /// Inputs are: receiver, its pose in local ref. frame. Outputs are force and torque applied. 
    virtual void ComputeForces( PhysicsItem * receiver, const State & st, Vector3d & F, Vector3d & P ) const;

};


}



#endif





