
#include "item_node.h"


namespace Ign
{

ItemNode::ItemNode( Context * ctx )
    : Component( ctx )
{
}

ItemNode::~ItemNode()
{

}

void ItemNode::setParent( ItemNode * itemNode )
{
    Vector3d r, v, w;
    Quaterniond q;
    relativeAll( itemNode, r, q, v, w );

    parent_ = SharedPtr<ItemNode>( itemNode );

    setR( r );
    setQ( q );
    setV( v );
    setW( w );
}

ItemNode * ItemNode::parent() const
{

}

void ItemNode::setR( const Vector3d & r )
{
    r_ = r;
}

void ItemNode::setQ( const Quaterniond & q )
{
    q_ = q;
}

Vector3d     ItemNode::relR() const
{
    return r_;
}

Quaterniond  ItemNode::relQ() const
{
    return q_;
}

Vector3d     ItemNode::relV() const
{
    return v_;
}

Vector3d     ItemNode::relW() const
{
    return w_;
}

void ItemNode::setV( const Vector3d & v )
{
    v_ = v;
}

void ItemNode::setW( const Vector3d & w )
{
    w_ = w;
}

bool ItemNode::relativePose( ItemNode * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging )
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.
    Vector3d    r = Vector3::ZERO;
    Quaterniond q = Quaternion::IDENTITY;

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<ItemNode *> allAncestorsA;
    allAncestorsA.clear();
    ItemNode * nodeA = this;
    do {
        allAncestorsA.push_back( nodeA );
        nodeA = nodeA->parent();
    } while ( nodeA );
    const size_t allQtyA = allAncestorsA.size();

    ItemNode * nodeB = other;
    static std::vector<ItemNode *> ancestorsB;
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
        nodeB = nodeB->parent();
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
        const Quaterniond q = nodeA->relQ();
        const Vector3d    r = nodeA->relR();
        ra = q*ra;
        ra = r + ra;
        qa = q * qa;
    }

    Vector3d    rb = Vector3d::ZERO;
    Quaterniond qb = Quaterniond::IDENTITY;
    const size_t indB = ancestorsB.size()-1;
    for ( size_t i=0; i<indB; i++ )
    {
        ItemNode * nodeB = ancestorsB[i];
        const Quaterniond q = nodeB->relQ();
        const Vector3d    r = nodeB->relR();
        rb = q*rb;
        rb = r + rb;
        qb = q * qb;
        if ( debugLogging )
        {
            //URHO3D_LOGINFOF( "Node %s", nodeB->GetName().CString() );
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

bool ItemNode::relativeAll( const ItemNode * other, Vector3d & rel_r, Quaterniond & rel_q,
                                                    Vector3d & rel_v, Vector3d & rel_w, bool debugLogging ) const
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.
    Vector3d    r = Vector3::ZERO;
    Quaterniond q = Quaternion::IDENTITY;

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static std::vector<const ItemNode *> allAncestorsA;
    allAncestorsA.clear();
    const ItemNode * itemA = this;
    do {
        allAncestorsA.push_back( itemA );
        itemA = itemA->parent();
    } while ( itemA );
    const size_t allQtyA = allAncestorsA.size();

    const ItemNode * itemB = other;
    static std::vector<const ItemNode *> ancestorsB;
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
        itemB = itemB->parent();
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
        const ItemNode * itemB = ancestorsB[i];
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
            //URHO3D_LOGINFOF( "Node %s", itemB->GetNode()->GetName().CString() );
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

}





