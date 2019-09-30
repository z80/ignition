
#ifndef __ITEM_NODE_H_
#define __ITEM_NODE_H_

#include "vector3d.h"
#include "quaterniond.h"
#include <Urho3D/Urho3DAll.h>

using namespace Urho3D;

namespace Ign
{

class ItemNode:  public Urho3D::Component
{
    URHO3D_OBJECT( ItemNode, Component )
public:
    typedef signed long long Timestamp;

    ItemNode( Context * ctx );
    virtual ~ItemNode();

    void setParent( ItemNode * itemNode );
    ItemNode * parent() const;

    virtual void setR( const Vector3d & r );
    virtual void setQ( const Quaterniond & q );
    Vector3d     relR() const;
    Quaterniond  relQ() const;

    Vector3d     relV() const;
    Vector3d     relW() const;

    virtual void setV( const Vector3d & v );
    virtual void setW( const Vector3d & w );

    bool relativePose( ItemNode * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging=false );
    bool relativeAll( const ItemNode * other, Vector3d & rel_r, Quaterniond & rel_q,
                                              Vector3d & rel_v, Vector3d & rel_w, bool debugLogging=false ) const;

    // "Teleporting" with preserving orientations and velocities of
    // all children in parent->parent ref. frame.
    virtual bool teleport( ItemNode * other, const Vector3d & r, const Quaterniond & q,
                                             const Vector3d & v=Vector3d::ZERO,
                                             const Vector3d & w=Vector3d::ZERO );

public:
    SharedPtr<ItemNode> parent_;
    /// Relative position.
    Vector3d    r_;
    /// Relative rotation.
    Quaterniond q_;
    /// Relative velocity.
    Vector3d    v_;
    /// Relative angular velocity.
    Vector3d    w_;

    /// So far this one is used only when teleporting.
    /// This one is just to aoid memory allocation/reallocation
    /// on every teleport.
    Vector<ItemNode *> children;

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



