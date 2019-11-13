
#include "ref_frame.h"


namespace Ign
{

static void removeFromList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children );
static void addToList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children );

RefFrame::RefFrame( Context * ctx, const String & name )
    : Component( ctx ),
      name_( name )
{
}

RefFrame::~RefFrame()
{

}

void RefFrame::setName( const String & name )
{
    name_ = name;
}
const String & RefFrame::name() const
{
    return name_;
}


void RefFrame::setParent( RefFrame * newParent )
{
    SharedPtr<RefFrame> & curParent = parent_;
    if ( newParent == curParent )
        return;

    // Compute relative parameters with respect to the new parent.
    State st;
    relativeState( newParent, st );

    // Replace the parent.
    if ( curParent )
        removeFromList( this, curParent->children_ );
    parent_ = SharedPtr<RefFrame>( newParent );
    if ( newParent )
        addToList( this, newParent->children_ );

    setR( st.r );
    setQ( st.q );
    setV( st.v );
    setW( st.w );
}

RefFrame * RefFrame::parent() const
{
    return parent_;
}

void RefFrame::setR( const Vector3d & r )
{
    st_.r = r;
}

void RefFrame::setQ( const Quaterniond & q )
{
    st_.q = q;
}

Vector3d     RefFrame::relR() const
{
    return st_.r;
}

Quaterniond  RefFrame::relQ() const
{
    return st_.q;
}

Vector3d     RefFrame::relV() const
{
    return st_.v;
}

Vector3d     RefFrame::relW() const
{
    return st_.w;
}

void RefFrame::setV( const Vector3d & v )
{
    st_.v = v;
}

void RefFrame::setW( const Vector3d & w )
{
    st_.w = w;
}

void RefFrame::setState( const State & st )
{
    setR( st.r );
    setQ( st.q );
    setV( st.v );
    setW( st.w );
}

const State RefFrame::state() const
{
    State st;
    st.r = relR();
    st.q = relQ();
    st.v = relV();
    st.w = relW();
    return st;
}

bool RefFrame::relativePose( RefFrame * other, Vector3d & rel_r, Quaterniond & rel_q, bool debugLogging ) const 
{
    State state;
    const bool res = relativeState( other, state, debugLogging );
    rel_r = state.r;
    rel_q = state.q;

    return res;
}

bool RefFrame::relativeState( const RefFrame * other, State & stateRel, bool debugLogging ) const
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static Vector<const RefFrame *> allAncestorsA;
    allAncestorsA.Clear();
    const RefFrame * itemA = this;
    while ( itemA )
    {
        allAncestorsA.Push( itemA );
        itemA = itemA->parent();
    }
    const unsigned allQtyA = allAncestorsA.Size();

    const RefFrame * itemB = other;
    static Vector<const RefFrame *> ancestorsB;
    ancestorsB.Clear();
    unsigned indA = allQtyA;
    while ( itemB )
    {
        // Check if nodeB is in allAncestorsA.
        for ( unsigned i=0; i<allQtyA; i++ )
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
        // Add to the list.
        ancestorsB.Push( itemB );
        // Get parent.
        itemB = itemB->parent();
    }

    // If reached the root and didn't meed
    // anything common just break.
    //if ( indA == allQtyA )
    //    return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3d    r_a = Vector3d::ZERO;
    Quaterniond q_a = Quaterniond::IDENTITY;
    Vector3d    v_a = Vector3d::ZERO;
    Vector3d    w_a = Vector3d::ZERO;
    for ( unsigned i=0; i<indA; i++ )
    {
        itemA = allAncestorsA[i];
        const Quaterniond q_n_1 = itemA->relQ();
        const Vector3d    r_n_1 = itemA->relR();
        const Vector3d    v_n_1 = itemA->relV();
        const Vector3d    w_n_1 = itemA->relW();
        // Due to ref. frame in Urho3D is left handed not
        // sure if here it should be "+ w.cross(r)" or "- w.cross(r)".
        v_a = v_n_1 + q_n_1*v_a + w_n_1.CrossProduct( q_n_1  * r_a );
        w_a = w_n_1 + q_n_1*w_a;

        r_a = r_n_1 + q_n_1*r_a;
        q_a = q_n_1 * q_a;
    }

    Vector3d    r_b = Vector3d::ZERO;
    Quaterniond q_b = Quaterniond::IDENTITY;
    Vector3d    v_b = Vector3d::ZERO;
    Vector3d    w_b = Vector3d::ZERO;
    const unsigned indB = ancestorsB.Size();
    for ( unsigned i=0; i<indB; i++ )
    {
        const RefFrame * itemB = ancestorsB[i];
        const Quaterniond q_n_1 = itemB->relQ();
        const Vector3d    r_n_1 = itemB->relR();
        const Vector3d    v_n_1 = itemB->relV();
        const Vector3d    w_n_1 = itemB->relW();
        v_b = v_n_1 + q_n_1*v_b + w_n_1.CrossProduct( q_n_1  * r_b );
        w_b = w_n_1 + q_n_1*w_b;

        r_b = r_n_1 + q_n_1*r_b;
        q_b = q_n_1 * q_b;

        if ( debugLogging )
        {
            //URHO3D_LOGINFOF( "Node %s", itemB->GetNode()->GetName().CString() );
            URHO3D_LOGINFOF( "     r: (%f, %f, %f)", r_n_1.x_, r_n_1.y_, r_n_1.z_ );
            URHO3D_LOGINFOF( "     q: (%f, %f, %f, %f)", q_n_1.w_, q_n_1.x_, q_n_1.y_, q_n_1.z_ );
        }
    }

    // Invert "B" movement.
    {
        const Quaterniond invQ = q_b.Inverse();
        const Vector3d    invR = -(invQ * r_b);
        const Vector3d    invW = -(invQ * w_b);
        const Vector3d    invV = -(invQ * v_b) - invW.CrossProduct( invR );
        q_b = invQ;
        r_b = invR;
        v_b = invV;
        w_b = invW;
    }

    // Apply once again to get final relative parameters.
    {
        stateRel.v = v_b + q_b*v_a + w_b.CrossProduct( q_b * r_a );
        stateRel.w = w_b + q_b*w_a;
        stateRel.r = r_b + q_b*r_a;
        stateRel.q = q_b * q_a;
    }

    return true;
}


bool RefFrame::relativeState( const RefFrame * other, const State & stateInOther, State & state, const bool debugLogging ) const
{
    // root->a->b->c->d->e->this
    // root->a->b->f->g->other
    // The idea is to accumulate relative position and orientation.

    // Get all ancestors of current node.
    // Make it static as graphics is in one thread.
    static Vector<const RefFrame *> allAncestorsA;
    allAncestorsA.Clear();
    const RefFrame * itemA = this;
    while ( itemA )
    {
        allAncestorsA.Push( itemA );
        itemA = itemA->parent();
    }
    const unsigned allQtyA = allAncestorsA.Size();

    const RefFrame * itemB = other;
    static Vector<const RefFrame *> ancestorsB;
    ancestorsB.Clear();
    unsigned indA = allQtyA;
    while ( itemB )
    {
        // Check if nodeB is in allAncestorsA.
        for ( unsigned i=0; i<allQtyA; i++ )
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
        // Add to the list.
        ancestorsB.Push( itemB );
        // Get parent.
        itemB = itemB->parent();
    }

    // If reached the root and didn't meed
    // anything common just break.
    //if ( indA == allQtyA )
    //    return false;

    // Here there is a closest common ancestor.
    // First find pose of nodeA in it's ref. frame.
    Vector3d    r_a = Vector3d::ZERO;
    Quaterniond q_a = Quaterniond::IDENTITY;
    Vector3d    v_a = Vector3d::ZERO;
    Vector3d    w_a = Vector3d::ZERO;
    for ( unsigned i=0; i<indA; i++ )
    {
        itemA = allAncestorsA[i];
        const Quaterniond q_n_1 = itemA->relQ();
        const Vector3d    r_n_1 = itemA->relR();
        const Vector3d    v_n_1 = itemA->relV();
        const Vector3d    w_n_1 = itemA->relW();
        // Due to ref. frame in Urho3D is left handed not
        // sure if here it should be "+ w.cross(r)" or "- w.cross(r)".
        v_a = v_n_1 + q_n_1*v_a + w_n_1.CrossProduct( q_n_1  * r_a );
        w_a = w_n_1 + q_n_1*w_a;

        r_a = r_n_1 + q_n_1*r_a;
        q_a = q_n_1 * q_a;
    }

    Vector3d    r_b = stateInOther.r;
    Quaterniond q_b = stateInOther.q;
    Vector3d    v_b = stateInOther.v;
    Vector3d    w_b = stateInOther.w;
    const size_t indB = ancestorsB.Size();
    for ( unsigned i=0; i<indB; i++ )
    {
        const RefFrame * itemB = ancestorsB[i];
        const Quaterniond q_n_1 = itemB->relQ();
        const Vector3d    r_n_1 = itemB->relR();
        const Vector3d    v_n_1 = itemB->relV();
        const Vector3d    w_n_1 = itemB->relW();
        v_b = v_n_1 + q_n_1*v_b + w_n_1.CrossProduct( q_n_1  * r_b );
        w_b = w_n_1 + q_n_1*w_b;

        r_b = r_n_1 + q_n_1*r_b;
        q_b = q_n_1 * q_b;

        if ( debugLogging )
        {
            //URHO3D_LOGINFOF( "Node %s", itemB->GetNode()->GetName().CString() );
            URHO3D_LOGINFOF( "     r: (%f, %f, %f)", r_n_1.x_, r_n_1.y_, r_n_1.z_ );
            URHO3D_LOGINFOF( "     q: (%f, %f, %f, %f)", q_n_1.w_, q_n_1.x_, q_n_1.y_, q_n_1.z_ );
        }
    }

    // Invert "B" movement.
    {
        const Quaterniond invQ = q_b.Inverse();
        const Vector3d    invR = -(invQ * r_b);
        const Vector3d    invW = -(invQ * w_b);
        const Vector3d    invV = -(invQ * v_b) - invW.CrossProduct( invR );
        q_b = invQ;
        r_b = invR;
        v_b = invV;
        w_b = invW;
    }

    // Apply once again to get final relative parameters.
    {
        state.v = v_b + q_b*v_a + w_b.CrossProduct( q_b * r_a );
        state.w = w_b + q_b*w_a;
        state.r = r_b + q_b*r_a;
        state.q = q_b * q_a;
    }

    return true;
}

bool RefFrame::teleport( RefFrame * other,
                         const Vector3d & r, const Quaterniond & q,
                         const Vector3d & v, const Vector3d & w )
{
    State stateInOther, st;
    stateInOther.r = r;
    stateInOther.q = q;
    stateInOther.v = v;
    stateInOther.w = w;

    const unsigned qty = children_.Size();
    static Vector<State> newStates;
    newStates.Clear();
    newStates.Reserve( qty );
    for ( unsigned i=0; i<qty; i++ )
    {
        RefFrame * n = children_[i];
        n->relativeState( other, stateInOther, st );
        newStates.Push( st );
    }
    setState( stateInOther );
    {
        const unsigned int qty = children_.Size();
        for ( unsigned int i=0; i<qty; i++ )
        {
            RefFrame * n = children_[i];
            const State & st = newStates[i];
            n->setState( st );
        }
    }

    return true;
}

static void removeFromList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children )
{
    bool repeat = false;
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





