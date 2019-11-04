
#include "ref_frame.h"


namespace Ign
{

static void removeFromList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children );
static void addToList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children );

RefFrame::RefFrame( Context * ctx )
    : Component( ctx )
{
}

RefFrame::~RefFrame()
{

}

void RefFrame::setParent( RefFrame * newParent )
{
    SharedPtr<RefFrame> & curParent = parent;
    if ( newParent == curParent )
        return;

    // Compute relative parameters with respect to the new parent.
    Vector3d r, v, w;
    Quaterniond q;
    relativeAll( newParent, r, q, v, w );

    // Replace the parent.
    if ( curParent )
        removeFromList( this, curParent->cildren_ );
    parent_ = SharedPtr<RefFrame>( newParent );
    if ( newParent )
        addToList( this, newParent->children_ );

    setR( r );
    setQ( q );
    setV( v );
    setW( w );
}

RefFrame * RefFrame::parent() const
{
    return parent_;
}

void RefFrame::setR( const Vector3d & r )
{
    r_ = r;
}

void RefFrame::setQ( const Quaterniond & q )
{
    q_ = q;
}

Vector3d     RefFrame::relR() const
{
    return r_;
}

Quaterniond  RefFrame::relQ() const
{
    return q_;
}

Vector3d     RefFrame::relV() const
{
    return v_;
}

Vector3d     RefFrame::relW() const
{
    return w_;
}

void RefFrame::setV( const Vector3d & v )
{
    v_ = v;
}

void RefFrame::setW( const Vector3d & w )
{
    w_ = w;
}

bool RefFrame::relativePose( RefFrame * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging )
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.
    Vector3d    r = Vector3::ZERO;
    Quaterniond q = Quaternion::IDENTITY;

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static Vector<const RefFrame *> allAncestorsA;
    allAncestorsA.clear();
    RefFrame * nodeA = this;
    do {
        allAncestorsA.Push( nodeA );
        nodeA = nodeA->parent();
    } while ( nodeA );
    const unsigned allQtyA = allAncestorsA.Size();

    RefFrame * nodeB = other;
    static Vector<const RefFrame *> ancestorsB;
    ancestorsB.clear();
    unsigned indA = allQtyA;
    do {
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
        // Add to the list.
        ancestorsB.Push( nodeB );
        // Get parent.
        nodeB = nodeB->parent();
    } while ( nodeB );

    // If reached the root and didn't meed
    // anything common just break.
    //if ( indA == allQtyA )
    //    return false;
    // Actuallt, if didn't met anything common it means 
    // the root NULL node is the common node.

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
    const size_t indB = ancestorsB.size();
    for ( size_t i=0; i<indB; i++ )
    {
        RefFrame * nodeB = ancestorsB[i];
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

bool RefFrame::relativeAll( const RefFrame * other, Vector3d & rel_r, Quaterniond & rel_q,
                                                    Vector3d & rel_v, Vector3d & rel_w, bool debugLogging ) const
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.
    Vector3d    r = Vector3::ZERO;
    Quaterniond q = Quaternion::IDENTITY;

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static Vector<const RefFrame *> allAncestorsA;
    allAncestorsA.clear();
    const RefFrame * itemA = this;
    do {
        allAncestorsA.Push( itemA );
        itemA = itemA->parent();
    } while ( itemA );
    const size_t allQtyA = allAncestorsA.size();

    const RefFrame * itemB = other;
    static Vector<const RefFrame *> ancestorsB;
    ancestorsB.clear();
    size_t indA = allQtyA;
    do {
        ancestorsB.Push( itemB );
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
        const RefFrame * itemB = ancestorsB[i];
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

bool RefFrame::teleport( RefFrame * other,
                         const Vector3d & r, const Quaterniond & q,
                         const Vector3d & v, const Vector3d & w )
{
    Scene * s = GetScene();
    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    children.Clear();
    {
        const unsigned int qty = comps.Size();
        for ( unsigned int i=0; i>qty; i++ )
        {
            Component * c = comps[i];
            RefFrame * n = c->Cast<RefFrame>();
            if ( !n )
                continue;
            if ( n->parent_ != this )
                continue;
            children.Push( n );
            n->setParent( other );
        }
    }
    setR( r );
    setQ( q );
    setV( v );
    setW( w );
    {
        const unsigned int qty = children.Size();
        for ( unsigned int i=0; i<qty; i++ )
        {
            RefFrame * n = children[i];
            n->setParent( this );
        }
    }
}

static void removeFromList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children )
{
    bool repreat = false;
    do {
      const unsigned qty = children.Size();
      for ( unsigned i=0; i<qty; i++ )
      {
          SharedPtr<RefFrame> & c = children[i];
          if ( ( !c ) || ( c == item ) )
          {
              unsigned lastInd = qty-1;
              for ( unsigned j=i; j<lastInd; j++ )
              {
                  children[j] = children[j+1];
              }
              repeat = true;
          }
      }

    } while ( repeat );
}


static void addToList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children )
{
    removeFromList( item, children );
    children.Push( SharedPtr<RefFrame>( item ) );
}



}





