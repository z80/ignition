
#ifndef __REF_FRAME_H_
#define __REF_FRAME_H_

#include "data_types.h"
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
    /// Register object factory.
    static void RegisterObject( Context * context);

    RefFrame( Context * ctx, const String & name=String() );
    virtual ~RefFrame();

    void setName( const String & name );
    const String & name() const;

    void setParent( RefFrame * newParent );
    RefFrame * parent() const;

    virtual void setR( const Vector3d & r );
    virtual void setQ( const Quaterniond & q );
    Vector3d     relR() const;
    Quaterniond  relQ() const;
    Vector3d     relV() const;
    Vector3d     relW() const;

    Vector3d     refR() const;
    Quaterniond  refQ() const;
    Vector3d     refV() const;
    Vector3d     refW() const;

    virtual void setV( const Vector3d & v );
    virtual void setW( const Vector3d & w );

    void setState( const State & st );
    const State state() const;

    bool relativePose( RefFrame * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging=false ) const;
    bool relativeState( const RefFrame * other, State & stateRel, bool debugLogging=false ) const;

    /// Relative state for object in current ref. frame with state "stA" 
    /// with respect to another object with state "stB" in "other" ref. frame.
    bool relativeState( const RefFrame * other, const State & stateInOther, State & state, const bool debugLogging=false ) const;

    // "Teleporting" with preserving orientations and velocities of
    // all children in parent->parent ref. frame.
    virtual bool teleport( RefFrame * other, const Vector3d & r, const Quaterniond & q,
                                             const Vector3d & v=Vector3d::ZERO,
                                             const Vector3d & w=Vector3d::ZERO );

    bool computeRefState( const RefFrame * other, Timestamp t, bool recursive=false );
    const State & refState() const;
    /// Default implementation does nothing.
    virtual void refStateChanged();
    /// On pose attribute update.
    /// This is called when powe attribute is changed over network.
    virtual void poseChanged();

    /// Attribute accessors.
    unsigned getParentId() const;
    void setParentId( unsigned parentId );

public:
    /// For debugging it is easier to identify by human readable name.
    String name_;
    /// Parent ref. frame or nullptr.
    SharedPtr<RefFrame> parent_;
    /// So far this one is used only when teleporting.
    /// This one is just to aoid memory allocation/reallocation
    /// on every teleport.
    Vector<SharedPtr<RefFrame> > children_;

    // All these are in parent's ref. frame.
    /// Relative state with respect to direct parent in its ref. frame.
    State st_;
    /// State with respect to specified ref. frame.
    State refSt_;
    /// Moment of time refSt_ has been computed.
    /// Used to not recompute if called recursively.
    Timestamp refT_;
    /// Relative position.
    //Vector3d    r_;
    /// Relative rotation.
    //Quaterniond q_;
    /// Relative velocity.
    //Vector3d    v_;
    /// Relative angular velocity.
    //Vector3d    w_;

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



