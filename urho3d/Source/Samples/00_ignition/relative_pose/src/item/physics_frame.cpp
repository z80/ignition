
#include "physics_frame.h"
#include "physics_item.h"
//#include "surface_collision_mesh.h"
#include "force_source_frame.h"
#include "settings.h"
#include "Notifications.h"

namespace Ign
{


static Float cluster( unsigned & splitInd, Vector<SharedPtr<RefFrame> > & src, Vector<SharedPtr<RefFrame> > & dest );
static Float clusterScore( Vector<SharedPtr<RefFrame> > & objs, unsigned splitInd );
static Float clusterDist( Vector<SharedPtr<RefFrame> > & objs, unsigned splitInd );
static void  clusterSwap( Vector<SharedPtr<RefFrame> > & src, Vector<SharedPtr<RefFrame> > & dest, unsigned & splitInd, unsigned & elementInd );


void PhysicsFrame::RegisterComponent( Context * context )
{
    context->RegisterFactory<PhysicsFrame>();
    URHO3D_COPY_BASE_ATTRIBUTES( RefFrame );
}


PhysicsFrame::PhysicsFrame( Context * context )
    : RefFrame( context )
{
    setName( "PhysicsFrame" );
}

PhysicsFrame::~PhysicsFrame()
{

}

void PhysicsFrame::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    RefFrame::DrawDebugGeometry( debug, depthTest );
    if ( physicsWorld_ )
        physicsWorld_->DrawDebugGeometry( debug, depthTest );
}

void PhysicsFrame::physicsStep( float sec_dt )
{
    if ( !physicsWorld_ )
        return;

    applyForces();

    physicsWorld_->Update( sec_dt );

    updateChildStates();
}

bool PhysicsFrame::handleSplitMerge()
{
    const Vector<SharedPtr<RefFrame> > & obs = userControlledObjects();
    const bool worthToExist = checkIfWorthToExist();
    if ( !worthToExist )
        return true;
    checkIfTeleport();
    checkInnerObjects();
    checkOuterObjects();
    if ( checkIfNeedToSplit() )
        return true;
    checkIfNeedToMerge();

    return false;
}

Node * PhysicsFrame::physicsNode()
{
    return node_;
}

void PhysicsFrame::OnSceneSet( Scene * scene )
{
    if ( !scene )
        return;

    Environment * env = this->env();
    if ( !env )
        return;

    const bool isServer = env->IsServer();
    if ( !isServer )
        return;

    const String name( "Dynamics node" );
    Node * n = scene->CreateChild( name, LOCAL );
    node_         = SharedPtr<Node>( n );
    physicsWorld_ = n->CreateComponent<PhysicsWorld2>( LOCAL );

    // Make smaller gravity for debugging.
    //physicsWorld_->SetGravity( Vector3( 0.0, -1.0, 0.0 ) );
    physicsWorld_->SetGravity( Vector3::ZERO );

    // Also create a surface collision mesh item.
    //SurfaceCollisionMesh * scm = scene->CreateComponent<SurfaceCollisionMesh>( LOCAL );
    //scm->setParent( this );
    //scm->setR( Vector3d::ZERO );
    //scm->constructCustomGeometry();
}

void PhysicsFrame::applyForces()
{
    // Pick parent object and check if it is a source of forces.
    RefFrame * p = parent();
    ForceSourceFrame * fsf = nullptr;
    while ( p )
    {
        fsf = p->Cast<ForceSourceFrame>();
        if ( fsf )
            break;
        p = p->parent();
    }
    if ( !fsf )
        return;

    const unsigned qty = children_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<RefFrame> o = children_[i];
        if ( !o )
            continue;
        PhysicsItem * pi = o->Cast<PhysicsItem>();
        if ( !pi )
            continue;
        RigidBody2 * rb = pi->rigidBody();
        if ( !rb )
            continue;
        const bool kinematic = rb->IsKinematic();
        if ( kinematic )
            continue;
        const Float m = rb->GetMass();
        if ( m <= 0.0 )
            continue;

        fsf->ApplyForces( pi );
    }
}

void PhysicsFrame::updateChildStates()
{
    const unsigned qty = children_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<RefFrame> o = children_[i];
        if ( !o )
            continue;
        PhysicsItem * pi = o->Cast<PhysicsItem>();
        if ( !pi )
            continue;
        pi->updateStateFromRigidBody();
    }
}

const Vector<SharedPtr<RefFrame> > & PhysicsFrame::userControlledObjects()
{
    // Select user controlled objects.
    userControlledList_.Clear();
    const unsigned qty = children_.Size();
    for ( unsigned i=0 ;i<qty; i++ )
    {
        SharedPtr<RefFrame> o = children_[i];
        if ( o->getUserControlled() )
            userControlledList_.Push( o );
    }
    return userControlledList_;
}

void PhysicsFrame::checkInnerObjects()
{
    if ( userControlledList_.Empty() )
        return;

    // For each object compute smallest distance to user controlled object.
    // If smallest distance is bigger than horizont distance exclude object from 
    // this ref. frame.
    while ( true )
    {
        bool doItAgain = false;
        const unsigned userQty = userControlledList_.Size();
        const unsigned qty = children_.Size();
        const Float removeDist = Settings::dynamicsWorldDistanceExclude();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<RefFrame> o = children_[i];
            const bool isUserControlled = o->getUserControlled();
            if ( isUserControlled )
                continue;
            Float minDist = -1.0;
            for ( unsigned j=0; j<userQty; j++ )
            {
                SharedPtr<RefFrame> userObj = userControlledList_[j];
                const Float dist = o->distance( userObj );
                if ( ( minDist < 0.0 ) || ( dist < minDist ) )
                    minDist = dist;
            }

            if ( minDist > removeDist )
            {
                o->setParent( parent_ );
                doItAgain = true;
                break;
            }
        }
        if ( !doItAgain )
            break;
    }
}

void PhysicsFrame::checkOuterObjects()
{
    // At the moment don't know how to deal with it.
    
    

    // This is over-simplification. Need to redo this in future (!!!).

    // Right now just check all parent's children and check if need to include
    // either one.
    SharedPtr<RefFrame> p = parent_;
    if ( p )
    {
        const Float includeDist = Settings::dynamicsWorldDistanceInclude();
        while ( true )
        {
            bool loopAgain = false;
            const Vector<SharedPtr<RefFrame> > & objs = p->children_;
            unsigned qty = objs.Size();

            for ( unsigned i=0; i<qty; i++ )
            {
                SharedPtr<RefFrame> o = objs[i];
                // Skip this node itself and any other physics frames.
                if ( o == this )
                    continue;
                PhysicsFrame * pf = o->Cast<PhysicsFrame>();
                if ( pf )
                    continue;

                const Float dist = o->distance( this );
                if ( dist <= includeDist )
                {
                    o->setParent( this );
                    loopAgain = true;
                    break;
                }
            }
            if ( !loopAgain )
                break;
        }
    }
    else
    {
        Scene * s = GetScene();
        while( true )
        {
            bool loopAgain = false;
            const Vector<SharedPtr<Component> > & comps = s->GetComponents();
            const Float includeDist = Settings::dynamicsWorldDistanceInclude();
            const unsigned compsQty = comps.Size();
            for ( unsigned i=0; i<compsQty; i++ )
            {
                Component * c = comps[i];
                if ( !c )
                    continue;
                PhysicsItem * pi = c->Cast<PhysicsItem>();
                if ( !pi )
                    continue;

                // Only consider objects with no parent
                // and ignore others.
                RefFrame * currentParent = pi->parent();
                if ( currentParent )
                    continue;

                const Float dist = pi->distance( this );
                if ( dist <= includeDist )
                {
                    pi->setParent( this );
                    loopAgain = true;
                    break;
                }
            }
            if ( !loopAgain )
                break;
        }
    }
}

bool PhysicsFrame::checkIfWorthToExist()
{
    // If there are user controlled objects keep existing.
    if ( !userControlledList_.Empty() )
        return true;

    // If there are no user objects remove this object.
    // But first parent all objects inside to the parent.
    const unsigned qty = children_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<RefFrame> o = children_[0];
        o->setParent( parent_ );
    }

    // Remove this physics ref. frame.
    this->Remove();

    return false;
}

void PhysicsFrame::checkIfTeleport()
{
    SharedPtr<RefFrame> o = userControlledList_.At(0);
    // Follow the first one.
    const Float teleportDist = Settings::teleportDistance();
    const Float d = o->distance();
    if ( d > teleportDist )
    {
        State st;
        o->relativeState( parent_, st );
        st.q = Quaterniond::IDENTITY;
        st.v = Vector3d::ZERO;
        st.w = Vector3d::ZERO;
        {
            const String stri = String( "Teleporting to: (" ) +
                                String( st.r.x_ ) + String( ", " ) +
                                String( st.r.y_ ) + String( ", " ) +
                                String( st.r.z_ ) + String( ")" );
            Notifications::AddNotification( GetContext(), stri );
        }
        teleport( parent_, st );
    }
}

bool PhysicsFrame::checkIfNeedToSplit()
{
    const unsigned qty = userControlledList_.Size();
    if ( qty < 2 )
        return false;
    unsigned splitInd;
    const Float dist = cluster( splitInd, userControlledList_, userControlledList2_ );
    const Float splitDist = Settings::dynamicsWorldDistanceExclude();
    if ( dist < splitDist )
        return false;

    // Create another physics frame and move
    // second cluster nodes to that ref. frame.
    Scene * s = GetScene();
    PhysicsFrame * pf = s->CreateComponent<PhysicsFrame>();
    RefFrame * p = parent_;
    if ( p )
        pf->setParent( parent_ );

    // Define the state of newly created physics frame.
    {
        const State s = this->state();
        pf->setState( s );
    }
    {
        SharedPtr<RefFrame> o = userControlledList_[splitInd];
        State relativeSt;
        o->relativeState( p, relativeSt );
        relativeSt.q = relQ();
        relativeSt.v = relV();
        relativeSt.w = relW();
        pf->setState( relativeSt );
    }

    // Move user controlled objects to the new physics frame.
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<RefFrame> o = userControlledList_[i];
        o->setParent( pf );
    }

    // All dynamic objects.
    // Measure distance to both origins and choose the closest one.
    {
        userControlledList_.Clear();
        userControlledList_ = children_;
        const unsigned qty = userControlledList_.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<RefFrame> o = userControlledList_[i];
            const bool userCtrled = o->getUserControlled();
            if ( userCtrled )
                continue;
            const Float distanceA = o->distance();
            const Float distanceB = o->distance( pf );
            if ( distanceB < distanceA )
                o->setParent( pf );
        }
    }

    return true;
}

bool PhysicsFrame::checkIfNeedToMerge()
{
    bool result = false;
    SharedPtr<RefFrame> p = parent_;
    const Float mergeDist = Settings::dynamicsWorldDistanceInclude();
    const Vector3d r = relR();
    if ( p )
    {
        userControlledList_ = p->children_;
        unsigned qty = userControlledList_.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            SharedPtr<RefFrame> o = userControlledList_[i];
            PhysicsFrame * pf = o->Cast<PhysicsFrame>();
            if ( !pf )
                continue;
            if ( pf == this )
                continue;
            const Float dist = pf->distance( r );
            if ( dist > mergeDist )
                continue;

            // Move all objects to a different physics frame.
            userControlledList2_ = pf->children_;
            const unsigned qty = userControlledList2_.Size();
            for ( unsigned j=0; j<qty; j++ )
            {
                SharedPtr<RefFrame> o = userControlledList2_[j];
                o->setParent( this );
            }
            pf->Remove();
            result = true;
        }
    }
    else
    {
        Scene * s = GetScene();
        if ( !s )
            return false;
        const Vector<SharedPtr<Component> > & comps = s->GetComponents();
        const unsigned compsQty = comps.Size();
        for ( unsigned i=0; i<compsQty; i++ )
        {
            Component * c = comps[i];
            if ( !c )
                continue;
            PhysicsFrame * pf = c->Cast<PhysicsFrame>();
            if ( !pf )
                continue;
            if ( pf == this )
                continue;
            // In this section check only frames with no parent.
            if ( pf->parent_ )
                continue;

            const Float dist = pf->distance( r );
            if ( dist > mergeDist )
                continue;

            // Move all objects to a different physics frame.
            userControlledList2_ = pf->children_;
            const unsigned qty = userControlledList2_.Size();
            for ( unsigned j=0; j<qty; j++ )
            {
                SharedPtr<RefFrame> o = userControlledList2_[j];
                o->setParent( this );
            }
            pf->Remove();
            result = true;
        }
    }

    return result;
}

static Float cluster( unsigned & splitInd, Vector<SharedPtr<RefFrame> > & src, Vector<SharedPtr<RefFrame> > & dest )
{
    const unsigned qty = src.Size();
    splitInd = qty;
    bool modified;
    Float baseScore = clusterScore( src, splitInd );
    do
    {
        // Reset number of modifications done.
        modified = false;

        for ( unsigned i=0; i<qty; i++ )
        {
            clusterSwap( src, dest, splitInd, i );
            const Float score = clusterScore( dest, splitInd );
            if ( score < baseScore )
            {
                src       = dest;
                baseScore = score;
                modified  = true;
                break;
            }
        }
    } while ( modified );

    const Float dist = clusterDist( src, splitInd );
    return dist;
}

static Float clusterScore( Vector<SharedPtr<RefFrame> > & objs, unsigned splitInd )
{
    Float score = 0.0;
    for ( unsigned i=0; i<splitInd; i++ )
    {
        SharedPtr<RefFrame> & oi = objs[i];
        const Vector3d ri = oi->relR();
        for ( unsigned j=0; j<splitInd; j++ )
        {
            if ( i == j )
                continue;
            SharedPtr<RefFrame> & oj = objs[j];
            const Float dist = oj->distance( ri );
            score += dist;
        }
    }

    const unsigned qty = objs.Size();
    for ( unsigned i=splitInd; i<qty; i++ )
    {
        SharedPtr<RefFrame> & oi = objs[i];
        const Vector3d ri = oi->relR();
        for ( unsigned j=splitInd; i<qty; j++ )
        {
            if ( i == j )
                continue;
            SharedPtr<RefFrame> & oj = objs[j];
            const Float dist = oj->distance( ri );
            score += dist;
        }
    }

    return score;
}


static Float clusterDist( Vector<SharedPtr<RefFrame> > & objs, unsigned splitInd )
{
    Float minDist = -1.0;
    const unsigned qty = objs.Size();
    for ( unsigned i=0; i<splitInd; i++ )
    {
        SharedPtr<RefFrame> & oi = objs[i];
        const Vector3d ri = oi->relR();
        for ( unsigned j=splitInd; j<qty; j++ )
        {
            SharedPtr<RefFrame> & oj = objs[j];
            const Vector3d rj = oj->relR();
            const Float distA = oj->distance( ri );
            const Float distB = oj->distance( rj );
            const Float dist = (distA + distB) * 0.5;
            if ( ( minDist < 0.0 ) || ( dist < minDist ) )
                minDist = dist;
        }
    }

    return minDist;
}

static void  clusterSwap( Vector<SharedPtr<RefFrame> > & src, Vector<SharedPtr<RefFrame> > & dest, unsigned & splitInd, unsigned & elementInd )
{
    dest = src;
    if ( elementInd < splitInd )
    {
        const unsigned lastInd = splitInd - 1;
        if ( elementInd < lastInd )
        {
            SharedPtr<RefFrame> a = dest[elementInd];
            SharedPtr<RefFrame> b = dest[lastInd];
            SharedPtr<RefFrame> tmp = b;
            dest[lastInd]    = a;
            dest[elementInd] = tmp;
        }
        splitInd -= 1;
    }
    else
    {
        const unsigned firstInd = splitInd;
        if ( firstInd < elementInd )
        {
            SharedPtr<RefFrame> a = dest[elementInd];
            SharedPtr<RefFrame> b = dest[firstInd];
            SharedPtr<RefFrame> tmp = b;
            dest[firstInd]   = a;
            dest[elementInd] = tmp;
        }
        splitInd += 1;
    }
}





}














