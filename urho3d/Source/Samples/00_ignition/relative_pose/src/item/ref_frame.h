
#ifndef __REF_FRAME_H_
#define __REF_FRAME_H_

#include "controllable_item.h"
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

class RefFrame:  public ControllableItem
{
    URHO3D_OBJECT( RefFrame, ControllableItem )
public:
    /// Register object factory.
    static void RegisterComponent( Context * context );

    RefFrame( Context * ctx, const String & name=String() );
    virtual ~RefFrame();

    void setName( const String & name );
    const String & name() const;

    void setParent( RefFrame * newParent );
    RefFrame * parent();

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

    bool relativePose( RefFrame * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging=false );
    bool relativeState( RefFrame * other, State & stateRel, bool debugLogging=false );

    /// Relative state for object in current ref. frame with state "stA" 
    /// with respect to another object with state "stB" in "other" ref. frame.
    bool relativeState( RefFrame * other, const State & stateInOther, State & state, const bool debugLogging=false );

    // "Teleporting" with preserving orientations and velocities of
    // all children in parent->parent ref. frame.
    virtual bool teleport( RefFrame * other, const State & stateInOther );

    bool computeRefState( RefFrame * other=nullptr, Timestamp t=-1, bool recursive=false );
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

    /// Attribute accessors.
    int getParentId() const;
    void setParentId( int parentId );

    bool getUserControlled() const;

    /// Distance to a point. This method is virtual because it needs to
    /// take into account this object size.
    virtual Float distance( RefFrame * refFrame );
    /// The same method computes a distance to a point in the same ref. frame.
    virtual Float distance( const Vector3d & r=Vector3d::ZERO ) const;

    /// Cleanup.
    /// Remove from parent's children list and deallocate any
    /// nodes, meshes etc.
    virtual void cleanup();

    /// Assign user variable to a node to be able to retrieve
    /// RefFrame when node is available.
    /// This is needed when for example selecting using mouse.
    void assignRefFrame( Node * node );
    static RefFrame * refFrame( Node * node );
    static RefFrame * refFrame( Scene * s, unsigned id );

public:
    void validateParentId();
    /// For debugging it is easier to identify by human readable name.
    String name_;
    /// Parent ref. frame or nullptr.
    SharedPtr<RefFrame> parent_;
    int parentId_;
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
};


}

#endif



