
#include "ref_frame.h"


namespace Ign
{

static void removeFromList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children );
static void addToList( RefFrame * item, Vector<SharedPtr<RefFrame> > & children );

void RefFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<RefFrame>();
    //URHO3D_COPY_BASE_ATTRIBUTES( Component );

    // Need to think on how to assign parent and update over network.
    URHO3D_ACCESSOR_ATTRIBUTE( "ParentId", getParentId, setParentId, unsigned, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "RfName", String, name_, "", AM_DEFAULT );

    URHO3D_ATTRIBUTE_EX( "Rx", double, st_.r.x_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE_EX( "Ry", double, st_.r.y_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE_EX( "Rz", double, st_.r.z_, poseChanged, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE_EX( "Qw", double, st_.q.w_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE_EX( "Qx", double, st_.q.x_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE_EX( "Qy", double, st_.q.y_, poseChanged, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE_EX( "Qz", double, st_.q.z_, poseChanged, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "Vx", double, st_.v.x_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Vy", double, st_.v.y_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Vz", double, st_.v.z_, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "Wx", double, st_.w.x_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Wy", double, st_.w.y_, 0.0, AM_DEFAULT );
    URHO3D_ATTRIBUTE( "Wz", double, st_.w.z_, 0.0, AM_DEFAULT );

    URHO3D_ATTRIBUTE( "Uc", bool, userControlled_, false, AM_DEFAULT );
}

RefFrame::RefFrame( Context * ctx, const String & name )
    : Component( ctx ),
      name_( name ),
      refT_( 0 ),
      userControlled_( false )
{
}

RefFrame::~RefFrame()
{
    cleanup();
}

void RefFrame::setName( const String & name )
{
    name_ = name;

    MarkNetworkUpdate();
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
    {
        leftRefFrame( curParent );
        removeFromList( this, curParent->children_ );
        curParent->childLeft( this );
    }
    parent_ = SharedPtr<RefFrame>( newParent );
    if ( newParent )
    {
        addToList( this, newParent->children_ );
        newParent->childEntered( this );
    }

    setState( st );
    if ( newParent )
        enteredRefFrame( newParent );

    MarkNetworkUpdate();
}

RefFrame * RefFrame::parent() const
{
    return parent_;
}

void RefFrame::setR( const Vector3d & r )
{
    st_.r = r;

    poseChanged();
    MarkNetworkUpdate();
}

void RefFrame::setQ( const Quaterniond & q )
{
    st_.q = q;

    poseChanged();
    MarkNetworkUpdate();
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

Vector3d     RefFrame::refR() const
{
    return refSt_.r;
}

Quaterniond  RefFrame::refQ() const
{
    return refSt_.q;
}

Vector3d     RefFrame::refV() const
{
    return refSt_.v;
}

Vector3d     RefFrame::refW() const
{
    return refSt_.w;
}

void RefFrame::setV( const Vector3d & v )
{
    st_.v = v;

    poseChanged();
    MarkNetworkUpdate();
}

void RefFrame::setW( const Vector3d & w )
{
    st_.w = w;

    poseChanged();
    MarkNetworkUpdate();
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
    State stateOther;
    const bool res = relativeState( other, stateOther, stateRel, debugLogging );

    return res;
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
            n->parentTeleported();
        }
    }
    if ( parent_ )
        parent_->childTeleported( this );

    return true;
}

bool RefFrame::computeRefState( const RefFrame * other, Timestamp t, bool recursive )
{
    if ( (refT_ != t) || (t < 0) )
    {
        // Compute for itself.
        const bool res = relativeState( other, refSt_ );
        // Call overrideable method with needed subclass functionality.
        refStateChanged();
        if ( !res )
            return false;
        refT_ = t;

        // Compute for all children if recursive.
        if ( recursive )
        {
            const unsigned qty = children_.Size();
            for ( unsigned i=0; i<qty; i++ )
            {
                RefFrame * c = children_[i];
                const bool resC = c->computeRefState( other, t );
                if ( !resC )
                    return false;
            }
        }
    }

    return true;
}

const State & RefFrame::refState() const
{
    return refSt_;
}

void RefFrame::refStateChanged()
{

}

void RefFrame::poseChanged()
{
    URHO3D_LOGINFO( "Pose changed" );
}

void RefFrame::enteredRefFrame( RefFrame * refFrame )
{

}

void RefFrame::leftRefFrame( RefFrame * refFrame )
{

}

void RefFrame::childEntered( RefFrame * refFrame )
{

}

void RefFrame::childLeft( RefFrame * refFrame )
{

}

void RefFrame::parentTeleported()
{

}

void RefFrame::childTeleported( RefFrame * refFrame )
{

}

void RefFrame::userControlledChanged( bool newUserControlled )
{

}

unsigned RefFrame::getParentId() const
{
    if ( !parent_ )
        return 0;
    const unsigned id = parent_->id_;
    return id;
}

void RefFrame::setParentId( unsigned parentId )
{
    Scene * s = this->GetScene();
    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const SharedPtr<Component> & c = comps[i];
        const unsigned compId = c->GetID();
        if ( compId == parentId )
        {
            RefFrame * p = c->Cast<RefFrame>();
            setParent( p );
            break;
        }
    }
}

bool RefFrame::getUserControlled() const
{
    return userControlled_;
}

void RefFrame::setUserControlled( bool userControlled )
{
    userControlledChanged( userControlled );
    userControlled_ = userControlled;
}

Float RefFrame::distance( RefFrame * refFrame ) const
{
    Vector3d rel_r;
    Quaterniond rel_q;
    relativePose( refFrame, rel_r, rel_q );
    const Float d = rel_r.Length();
    return d;
}

Float RefFrame::distance( const Vector3d & r ) const
{
    const Vector3d dr = st_.r - r;
    const Float d = dr.Length();
    return d;
}

void RefFrame::cleanup()
{
    if ( parent_ )
        setParent( 0 );
}

static const StringHash P_PARENT_ID( "ParentId" );

void RefFrame::assignRefFrame( Node * node )
{
    node->SetVar( P_PARENT_ID, Variant( this->GetID() ) );
}

RefFrame * RefFrame::refFrame( Node * node )
{
    const VariantMap & vm = node->GetVars();
    VariantMap::ConstIterator it = vm.Find( P_PARENT_ID );
    if ( it == vm.End() )
        return nullptr;

    const Variant v = it->second_;
    const unsigned id = v.GetUInt();

    Scene * s = node->GetScene();

    RefFrame * rf = refFrame( s, id );
    return rf;
}

RefFrame * RefFrame::refFrame( Scene * s, unsigned id )
{
    if ( !s )
        return nullptr;

    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned qty = comps.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Component * c = comps[i];
        const unsigned c_id = c->GetID();
        if ( id == c_id )
        {
            RefFrame * rf = c->Cast<RefFrame>();
            if ( rf )
                return rf;
        }
    }
    return nullptr;
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





