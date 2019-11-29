
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
    static void RegisterObject( Context * context );

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
    virtual bool teleport( RefFrame * other, const Vector3d & r, const Quaterniond & q=Quaterniond::IDENTITY,
                                             const Vector3d & v=Vector3d::ZERO,
                                             const Vector3d & w=Vector3d::ZERO );

    bool computeRefState( const RefFrame * other=nullptr, Timestamp t=-1, bool recursive=false );
    const State & refState() const;
    /// Default implementation does nothing.
    virtual void refStateChanged();
    /// On pose attribute update.
    /// This is called when pose attribute is changed over network.
    virtual void poseChanged();

    /// Called when this thing is moved into another ref. frame.
    virtual void enteredRefFrame( RefFrame * refFrame );
    /// Called when this thing is moved out of it's current parent.
    virtual void leftRefFrame( RefFrame * refFrame );
    /// Called when new chiled entered.
    virtual void childEntered( RefFrame * refFrame );
    /// Called when child lest.
    virtual void childLeft( RefFrame * refFrame );
    /// Called when parent teleported.
    virtual void parentTeleported();
    /// Called when child teleported.
    virtual void childTeleported( RefFrame * refFrame );

    /// When user controlled flag changes call this method.
    /// Called just before changing appropriate field so there
    /// is a way to know what it was before.
    virtual void userControlledChanged( bool newUserControlled );

    /// Attribute accessors.
    unsigned getParentId() const;
    void setParentId( unsigned parentId );

    bool getUserControlled() const;
    void setUserControlled( bool userControlled );

    /// Distance to a point. This method is virtual because it needs to
    /// take into account this object size.
    virtual Float distance( RefFrame * refFrame ) const;
    /// The same method computes a distance to a point in the same ref. frame.
    virtual Float distance( const Vector3 & r=Vector3d::ZERO ) const;

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
    /// User controlled.
    bool userControlled_;
};


}

#endif



