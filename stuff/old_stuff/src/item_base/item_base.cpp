
#include "item_base.h"
#include "name_generator.h"
#include "physics_world_2.h"

#include "rigid_body_2.h"
#include "collision_shape_2.h"
#include "physics_world_2.h"
#include "physics_events_2.h"

#include "SceneManager.h"


using namespace Urho3D;

namespace Osp
{

ItemBase::ItemBase( Context * context )
    : LogicComponent( context )
{
    _type     = TUnspecified;
}

ItemBase::~ItemBase()
{
}

ItemBase::Type ItemBase::type()
{
    return _type;
}

void ItemBase::Start()
{
    Node * n = GetNode();
    const Vector3 & r = n->GetPosition();
    this->r = Vector3d( r.x_, r.y_, r.z_ );
}

/*void ItemBase::subscribeToParentChanges()
{
    //SubscribeToEvent( E_NODEREMOVED, URHO3D_HANDLER( ItemBase, OnNodeRemoved ) );
    //SubscribeToEvent( E_NODEADDED,   URHO3D_HANDLER( ItemBase, OnNodeAdded ) );

    //toWorld();
}*/

void ItemBase::fromWorld()
{
    RigidBody2 * rb = rigidBody();
    if ( rb )
        rb->Remove();

    CollisionShape2 * cs = collisionShape();
    if ( cs )
        cs->Remove();

    Node * n = GetNode();
    PhysicsWorld2 * w = ItemBase::getWorld( n );
    if ( !w )
        return;

    // Stop updating velocity and position.
    UnsubscribeFromEvent( w, E_PHYSICSPOSTSTEP_2 );
}

void ItemBase::toWorld()
{
    // Subscribing to world updates to track body state.
    Node * n = GetNode();
    PhysicsWorld2 * w = ItemBase::getWorld( n );
    if ( !w )
        return;

    // This is to update position and velocity values.
    SubscribeToEvent(w, E_PHYSICSPOSTSTEP_2, URHO3D_HANDLER( ItemBase, OnPhysicsPostStep ) );
}

RigidBody2 * ItemBase::rigidBody()
{
    Node * n = GetNode();
    RigidBody2 * rb = n->GetComponent<RigidBody2>();
    return rb;
}

CollisionShape2 * ItemBase::collisionShape()
{
    Node * n = GetNode();
    CollisionShape2 * cs = n->GetComponent<CollisionShape2>();
    return cs;
}


void ItemBase::setR( const Vector3d & new_r )
{
    r = new_r;

    Node * n = GetNode();
    const Vector3 rf( r.x_, r.y_, r.z_ );
    n->SetPosition( rf );
}

Vector3d ItemBase::relR() const
{
    return r;
}

void ItemBase::setQ( const Quaterniond & new_q )
{
    q = new_q;

    Node * n = GetNode();
    const Quaternion qf( q.w_, q.x_, q.y_, q.z_ );
    n->SetRotation( qf );
}

Quaterniond ItemBase::relQ() const
{
    return q;
}

void ItemBase::setV( const Vector3d & new_v )
{
    v = new_v;
}

Vector3d ItemBase::relV() const
{
    return v;
}

void ItemBase::setW( const Vector3d & new_w )
{
    w = new_w;
}

Vector3d ItemBase::relW() const
{
    return w;
}

Vector3 ItemBase::relRf() const
{
    const Vector3d r = relR();
    const Vector3 rf( v.x_, v.y_, v.z_ );
    return rf;
}

Quaternion ItemBase::relQf() const
{
    const Quaterniond q = relQ();
    const Quaternion  qf( q.w_, q.x_, q.y_, q.z_ );
    return qf;
}

Vector3 ItemBase::relVf() const
{
    const Vector3d v = relV();
    const Vector3 vf( v.x_, v.y_, v.z_ );
    return vf;
}

Vector3 ItemBase::relWf() const
{
    const Vector3d w = relW();
    const Vector3 wf( w.x_, w.y_, w.z_ );
    return wf;
}

void ItemBase::setParent( ItemBase * parent, bool inheritRotation )
{
    Vector3    rel_r;
    Quaternion rel_q;
    const bool relPoseOk = relativePose( parent, rel_r, rel_q );

    GetNode()->SetParent( parent->GetNode() );

    if ( relPoseOk )
    {
        setR( rel_r );
        setQ( rel_q );
    }
}

void ItemBase::setParent( Node * parent, bool inheritRotation )
{
    Vector3    rel_r;
    Quaternion rel_q;
    const bool relPoseOk = relativePose( parent, rel_r, rel_q );

    GetNode()->SetParent( parent );

    if ( relPoseOk )
    {
        setR( rel_r );
        setQ( rel_q );
    }
}

bool ItemBase::isParentOf( ItemBase * item ) const
{
    Node * n = GetNode();
    Scene * scene = GetScene();
    const Node * root = scene;
    Node * node = item->GetNode();
    while ( ( node != root ) && ( node ) )
    {
        if ( n == node )
            return true;
        node = node->GetParent();
    }

    return false;
}

ItemBase * ItemBase::parentItem() const
{
    Node * n = GetNode()->GetParent();
    if ( !n )
        return 0;

    static Vector< SharedPtr<Component> > comps;
    comps = n->GetComponents();
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        ItemBase * ib = comps[i]->Cast<ItemBase>();
        if ( !ib )
            continue;
        return ib;
    }

    return nullptr;
}

ItemBase * ItemBase::headItem( const ItemBase * item )
{
    Node * n = item->GetNode();
    static Vector< SharedPtr<Component> > comps;
    comps = n->GetComponents();
    const size_t qty = comps.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        ItemBase * ib = comps[i]->Cast<ItemBase>();
        if ( !ib )
            continue;
        return ib;
    }

    return nullptr;
}

bool ItemBase::relativePose( const ItemBase * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging )
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.
    Vector3d    r = Vector3::ZERO;
    Quaterniond q = Quaternion::IDENTITY;

    const Node * root = GetScene();

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<Node *> allAncestorsA;
    allAncestorsA.clear();
    Node * nodeA = this->GetNode();
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->GetParent();
    } while ( nodeA );
    const size_t allQtyA = allAncestorsA.size();

    Node * nodeB = other->GetNode();
    static std::vector<Node *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( nodeB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            nodeA = allAncestorsA[i];
            if ( nodeA == nodeB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        nodeB = nodeB->GetParent();
    } while ( nodeB );

    // If reached the root and didn't meed
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3d    ra = Vector3d::ZERO;
    Quaterniond qa = Quaterniond::IDENTITY;
    for ( size_t i=0; i<indA; i++ )
    {
        nodeA = allAncestorsA[i];
        const Quaterniond q = nodeA->GetRotation();
        const Vector3d    r = nodeA->GetPosition();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Vector3d    rb = Vector3d::ZERO;
    Quaterniond qb = Quaterniond::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        Node * nodeB = ancestorsB[i];
        const Quaterniond q = nodeB->GetRotation();
        const Vector3d    r = nodeB->GetPosition();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
        if ( debugLogging )
        {
            URHO3D_LOGINFOF( "Node %s", nodeB->GetName().CString() );
            URHO3D_LOGINFOF( "     r: (%f, %f, %f)", r.x_, r.y_, r.z_ );
            URHO3D_LOGINFOF( "     q: (%f, %f, %f, %f)", q.w_, q.x_, q.y_, q.z_ );
        }
    }

    rel_r = qb.Inverse()* (ra - rb);
    // This might be wrong.
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;
    return true;
}

bool ItemBase::relativePose( const ItemBase * other, Vector3 & rel_r, Quaternion & rel_q )
{
    Vector3d    r;
    Quaterniond q;
    const bool res = relativePose( other, r, q );
    rel_r = Vector3( r.x_, r.y_, r.z_ );
    rel_q = Quaternion( q.w_, q.x_, q.y_, q.z_ );
    return res;
}

bool ItemBase::relativePose( const Node * other, Vector3d & rel_r, Quaterniond & rel_q )
{
    // root->a->b->c->d->e->this
    // root->a->b->other->f->g
    // The idea is to accumulate relative position and orientation.
    Vector3    r = Vector3::ZERO;
    Quaternion q = Quaternion::IDENTITY;

    const Node * root = GetScene();

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<const Node *> allAncestorsA;
    allAncestorsA.clear();
    const Node * nodeA = this->GetNode();
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->GetParent();
    } while ( nodeA );
    const size_t allQtyA = allAncestorsA.size();

    const Node * nodeB = other;
    static std::vector<const Node *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( nodeB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            nodeA = allAncestorsA[i];
            if ( nodeA == nodeB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        nodeB = nodeB->GetParent();
    } while ( nodeB );

    // If reached the root and didn't meed
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3    ra = Vector3::ZERO;
    Quaternion qa = Quaternion::IDENTITY;
    for ( size_t i=0; i<indA; i++ )
    {
        nodeA = allAncestorsA[i];
        const Quaternion q = nodeA->GetRotation();
        const Vector3    r = nodeA->GetPosition();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Vector3    rb = Vector3::ZERO;
    Quaternion qb = Quaternion::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        const Node * nodeB = ancestorsB[i];
        const Quaternion q = nodeB->GetRotation();
        const Vector3    r = nodeB->GetPosition();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
    }

    rel_r = qb.Inverse()* (ra - rb);
    // This might be wrong.
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;
    return true;
}

bool ItemBase::relativePose( const Node * other, Vector3  & rel_r, Quaternion  & rel_q )
{
    Vector3d    r;
    Quaterniond q;
    const bool res = relativePose( other, r, q );
    rel_r = Vector3( r.x_, r.y_, r.z_ );
    rel_q = Quaternion( q.w_, q.x_, q.y_, q.z_ );
    return res;
}

bool ItemBase::relativePose( const Node * n, const Node * p, Vector3 & rel_r, Quaternion & rel_q )
{
    // root->a->b->c->d->e->this
    // root->a->b->other->f->g
    // The idea is to accumulate relative position and orientation.
    Vector3    r = Vector3::ZERO;
    Quaternion q = Quaternion::IDENTITY;

    const Node * root = n->GetScene();

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<const Node *> allAncestorsA;
    allAncestorsA.clear();
    const Node * nodeA = n;
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->GetParent();
    } while ( nodeA );
    const size_t allQtyA = allAncestorsA.size();

    const Node * nodeB = p;
    static std::vector<const Node *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( nodeB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            nodeA = allAncestorsA[i];
            if ( nodeA == nodeB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        nodeB = nodeB->GetParent();
    } while ( nodeB );

    // If reached the root and didn't meed
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3    ra = Vector3::ZERO;
    Quaternion qa = Quaternion::IDENTITY;
    for ( size_t i=0; i<indA; i++ )
    {
        nodeA = allAncestorsA[i];
        const Quaternion q = nodeA->GetRotation();
        const Vector3    r = nodeA->GetPosition();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Vector3    rb = Vector3::ZERO;
    Quaternion qb = Quaternion::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        const Node * nodeB = ancestorsB[i];
        const Quaternion q = nodeB->GetRotation();
        const Vector3    r = nodeB->GetPosition();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
    }

    rel_r = qb.Inverse()* (ra - rb);
    // This might be wrong.
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;
    return true;
}

bool ItemBase::relativeAll( const ItemBase * other, Vector3d & rel_r, Quaterniond & rel_q,
                                              Vector3d & rel_v, Vector3d & rel_w, bool debugLogging ) const
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.
    Vector3d    r = Vector3::ZERO;
    Quaterniond q = Quaternion::IDENTITY;

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<const ItemBase *> allAncestorsA;
    allAncestorsA.clear();
    const ItemBase * itemA = headItem( this );
    do {
        allAncestorsA.push_back( itemA );
        itemA = itemA->parentItem();
    } while ( itemA );
    const size_t allQtyA = allAncestorsA.size();

    const ItemBase * itemB = headItem( other );
    static std::vector<const ItemBase *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.push_back( itemB );
        // Check if nodeB is in allAncestorsA.
        for ( size_t i=0; i<allQtyA; i++ )
        {
            itemA = allAncestorsA[i];
            if ( itemA == itemB )
            {
                indA = i;
                break;
            }
        }
        if ( indA != allQtyA )
            break;
        // Get parent.
        itemB = itemB->parentItem();
    } while ( itemB );

    // If reached the root and didn't meed
    // anything common just break.
    if ( indA == allQtyA )
        return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3d    ra = Vector3d::ZERO;
    Quaterniond qa = Quaterniond::IDENTITY;
    Vector3d    va = Vector3d::ZERO;
    Vector3d    wa = Vector3d::ZERO;
    for ( size_t i=0; i<indA; i++ )
    {
        itemA = allAncestorsA[i];
        const Quaterniond q = itemA->relQ();
        const Vector3d    r = itemA->relR();
        const Vector3d    v = itemA->relV();
        const Vector3d    w = itemA->relW();
        ra = q*ra;
        ra = r + ra;

        va = q*va;
        // Due to ref. frame in Urho3D is left handed not
        // sure if here it should be "+ w.cross(r)" or "- w.cross(r)".
        va = v + w.CrossProduct(r) + va;

        wa = q*wa;
        wa = w + wa;

        qa = q * qa;
    }

    Vector3d    rb = Vector3d::ZERO;
    Quaterniond qb = Quaterniond::IDENTITY;
    Vector3d    vb = Vector3d::ZERO;
    Vector3d    wb = Vector3d::ZERO;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        const ItemBase * itemB = ancestorsB[i];
        const Quaterniond q = itemB->relQ();
        const Vector3d    r = itemB->relR();
        const Vector3d    v = itemB->relV();
        const Vector3d    w = itemB->relW();
        rb = q*rb;
        rb = r + rb;

        vb = q*vb;
        // Due to ref. frame in Urho3D is left handed not
        // sure if here it should be "+ w.cross(r)" or "- w.cross(r)".
        vb = v + w.CrossProduct(r) + vb;

        wb = q*wb;
        wb = w + wb;

        qb = q * qb;
        if ( debugLogging )
        {
            URHO3D_LOGINFOF( "Node %s", itemB->GetNode()->GetName().CString() );
            URHO3D_LOGINFOF( "     r: (%f, %f, %f)", r.x_, r.y_, r.z_ );
            URHO3D_LOGINFOF( "     q: (%f, %f, %f, %f)", q.w_, q.x_, q.y_, q.z_ );
        }
    }

    const Quaterniond invQb = qb.Inverse();
    rel_r = invQb * (ra - rb);
    rel_v = invQb * (va - vb);
    rel_w = invQb * (wa - wb);
    // This might be wrong.
    // I probably don't need quaternion at all.
    rel_q = qb.Inverse() * qa;

    return true;
}

/*void ItemBase::OnNodeRemoved( StringHash eventType, VariantMap & eventData )
{
    Variant & n = eventData[NodeRemoved::P_NODE];
    Node * self = static_cast<Node *>( n.GetPtr() );
    Node * node = GetNode();
    if ( self != node )
        return;
    Variant & v = eventData[NodeRemoved::P_PARENT] ;
    Node * msgParent = static_cast<Node *>( v.GetPtr() );
    Node * parent = GetNode()->GetParent();
    if ( parent != msgParent )
        return;
    fromWorld();
}

void ItemBase::OnNodeAdded( StringHash eventType, VariantMap & eventData )
{
    Variant & n = eventData[NodeAdded::P_NODE];
    Node * self = static_cast<Node *>( n.GetPtr() );
    Node * node = GetNode();
    if ( self != node )
        return;
    Variant & v = eventData[NodeAdded::P_PARENT] ;
    Node * msgParent = static_cast<Node *>( v.GetPtr() );
    Node * parent = GetNode()->GetParent();
    if ( parent != msgParent )
        return;
    toWorld();
}*/

void ItemBase::OnPhysicsPostStep( StringHash t, VariantMap & e )
{
    RigidBody2 * rb = rigidBody();
    if ( !rb )
        return;
    // Need to update dynamical properties (probably...).
    setR( rb->GetPositiond() );
    setQ( rb->GetRotationd() );
    setV( rb->GetLinearVelocityd() );
    setW( rb->GetAngularVelocityd() );
}

PhysicsWorld2 * ItemBase::getWorld( Node * node )
{
    Node * pn = node->GetParent();
    if ( !pn )
        return nullptr;
    PhysicsWorld2 * w = pn->GetComponent<PhysicsWorld2>();
    return w;
}





}





