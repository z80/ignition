
#ifndef __REF_FRAME_H_
#define __REF_FRAME_H_

#include "vector3d.h"
#include "quaterniond.h"
#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;

namespace Ign
{

struct State
{
    Quaterniond q;
    Vector3d    r;
    Vector3d    v;
    Vector3d    w;
};

class RefFrame:  public Urho3D::Component
{
    URHO3D_OBJECT( RefFrame, Component )
public:
    typedef signed long long Timestamp;

    RefFrame( Context * ctx );
    virtual ~RefFrame();

    void setParent( RefFrame * newParent );
    RefFrame * parent() const;

    virtual void setR( const Vector3d & r );
    virtual void setQ( const Quaterniond & q );
    Vector3d     relR() const;
    Quaterniond  relQ() const;

    Vector3d     relV() const;
    Vector3d     relW() const;

    virtual void setV( const Vector3d & v );
    virtual void setW( const Vector3d & w );

    bool relativePose( RefFrame * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging=false ) const;
    bool relativeState( const RefFrame * other, State & stateRel, bool debugLogging=false ) const;

    /// Relative state for object in current ref. frame with state "stA" 
    /// with respect to another object with state "stB" in "other" ref. frame.
    bool relativeState( const RefFrame * other, const State & stateInOther, const State & state ) const;

    // "Teleporting" with preserving orientations and velocities of
    // all children in parent->parent ref. frame.
    virtual bool teleport( RefFrame * other, const Vector3d & r, const Quaterniond & q,
                                             const Vector3d & v=Vector3d::ZERO,
                                             const Vector3d & w=Vector3d::ZERO );

public:
    SharedPtr<RefFrame> parent_;
    /// So far this one is used only when teleporting.
    /// This one is just to aoid memory allocation/reallocation
    /// on every teleport.
    Vector<RefFrame *> children_;

    // All these are in parent's ref. frame.
    /// Relative position.
    Vector3d    r_;
    /// Relative rotation.
    Quaterniond q_;
    /// Relative velocity.
    Vector3d    v_;
    /// Relative angular velocity.
    Vector3d    w_;

//    /// Timestamp to check if need to recompute relative pose.
//    Timestamp   t_;
//    /// Origin with respect to which values are computed.
//    ItemNode  * origin_;
//    /// Position relative to origin.
//    Vector3d   ro_;
//    Quaternion qo_;
//    Vector3d   vo_;
//    Vector3d   wo_;
};


}

#endif



