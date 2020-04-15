
#include "physics_frame.h"
#include "physics_item.h"
#include "surface_collision_mesh.h"
#include "force_source_frame.h"
#include "camera_frame.h"
#include "settings.h"
#include "Notifications.h"

namespace Ign
{


static Float cluster( RefFrame * rf, unsigned & splitInd, Vector<unsigned> & src, Vector<unsigned> & dest );
static Float clusterScore( RefFrame * rf, Vector<unsigned> & objs, unsigned splitInd );
static Float clusterDist( RefFrame * rf, Vector<unsigned> & objs, unsigned splitInd );
static void  clusterSwap( RefFrame * rf, Vector<unsigned> & src, Vector<unsigned> & dest, unsigned & splitInd, unsigned & elementInd );


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
    if ( node_ )
        node_.Reset();
    if ( physicsWorld_ )
        physicsWorld_.Reset();
    if ( debug_override_node_ )
        debug_override_node_.Reset();

    // This is temporary holder.
    userControlledList_.Clear();
    userControlledList2_.Clear();
}

void PhysicsFrame::DrawDebugGeometry( DebugRenderer * debug, bool depthTest )
{
    RefFrame::DrawDebugGeometry( debug, depthTest );
    if ( physicsWorld_ )
    {
        physicsWorld_->SetDebugOrigin( debug_override_node_ );
        physicsWorld_->DrawDebugGeometry( debug, depthTest );
    }
}

void PhysicsFrame::refStateChanged()
{
    RefFrame::refStateChanged();

    const Vector3    r = refR().vector3();
    const Quaternion q = refQ().quaternion();
    if ( debug_override_node_ )
        debug_override_node_->SetTransform( r, q );
}

void PhysicsFrame::physicsStep( float sec_dt )
{
    if ( !physicsWorld_ )
        return;

    const bool surfOk = surfaceOk();
    if ( !surfOk )
        return;

    applyForces();

    physicsWorld_->Update( sec_dt );

    updateChildStates();
}

bool PhysicsFrame::handleSplitMerge()
{
    const Vector<unsigned> & obs = userControlledObjects();
    const bool worthToExist = checkIfWorthToExist();
    if ( !worthToExist )
        return true;
    checkIfTeleport();
    checkInnerObjects();
    checkOuterObjects();
    if ( checkIfNeedToSplit() )
        return true;
    if ( checkIfNeedToMerge() )
        return true;

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
    SurfaceCollisionMesh * scm = scene->CreateComponent<SurfaceCollisionMesh>( LOCAL );
    scm->setParent( this );
    scm->setR( Vector3d::ZERO );
    scm->constructCustomGeometry();

    // For frawing debug geometry.
    debug_override_node_ = scene->CreateChild( "Physics Debug Override Node" );
}

bool PhysicsFrame::surfaceOk()
{
    const unsigned qty = children_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        const unsigned chId = children_[i];
        RefFrame * o = refFrame( chId );
        if ( !o )
            continue;
        SurfaceCollisionMesh * scm = o->Cast<SurfaceCollisionMesh>();
        if ( !scm )
            continue;

        const bool valid = scm->valid();
        if ( !valid )
            return false;
    }

    return true;
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
        const unsigned chId = children_[i];
        RefFrame * o = refFrame( chId );
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
        const unsigned chId = children_[i];
        RefFrame * o = refFrame( chId );
        if ( !o )
            continue;
        PhysicsItem * pi = o->Cast<PhysicsItem>();
        if ( !pi )
            continue;
        pi->updateStateFromRigidBody();
    }
}

const Vector<unsigned> & PhysicsFrame::userControlledObjects()
{
    // Select user controlled objects.
    userControlledList_.Clear();
    const unsigned qty = children_.Size();
    for ( unsigned i=0 ;i<qty; i++ )
    {
        const unsigned chId = children_[i];
        RefFrame * o = refFrame( chId );
        if ( !o )
            continue;
        if ( o->getUserControlled() )
            userControlledList_.Push( chId );
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
            const unsigned chId = children_[i];
            RefFrame * o = refFrame( chId  );
            if ( !o )
                continue;
            const bool isUserControlled = o->getUserControlled();
            if ( isUserControlled )
                continue;
            // Also, if it is camera object, don't handle it.
            CameraFrame * cf = o->Cast<CameraFrame>();
            if ( cf )
                continue;
            Float minDist = -1.0;
            for ( unsigned j=0; j<userQty; j++ )
            {
                const unsigned chId = userControlledList_[j];
                RefFrame * userObj = refFrame( chId );
                const unsigned userObjId = userObj->GetID();
                const Float dist = o->distance( userObjId );
                if ( ( minDist < 0.0 ) || ( dist < minDist ) )
                    minDist = dist;
            }

            if ( minDist > removeDist )
            {
                o->setParent( parent_id_ );
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
    RefFrame * p = parent();
    if ( p )
    {
        const Float includeDist = Settings::dynamicsWorldDistanceInclude();
        while ( true )
        {
            bool loopAgain = false;
            const Vector<unsigned> & objs = p->children_;
            unsigned qty = objs.Size();

            for ( unsigned i=0; i<qty; i++ )
            {
                const unsigned objId = objs[i];
                RefFrame * o = refFrame( objId );
                // Skip this node itself and any other physics frames.
                if ( o == this )
                    continue;
                PhysicsFrame * pf = o->Cast<PhysicsFrame>();
                if ( pf )
                    continue;

                const unsigned thisId = this->GetID();
                const Float dist = o->distance( thisId );
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

                const unsigned thisId = this->GetID();
                const Float dist = pi->distance( thisId );
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
    // But destroy SurfaceCollisionMesh.
    const unsigned qty = children_.Size();
    static Vector<unsigned> children_s;
    children_s = children_;
    Scene * s = GetScene();
    for ( unsigned i=0; i<qty; i++ )
    {
        const unsigned chId = children_s[i];
        RefFrame * o = refFrame( chId );
        SurfaceCollisionMesh * scm = o->Cast<SurfaceCollisionMesh>();
        if ( scm )
        {
            scm->setParent( nullptr );
            s->RemoveComponent( scm );
            continue;
        }
        o->setParent( parent_id_ );
    }

    // Remove this physics ref. frame.
    this->setParent( nullptr );
    s->RemoveComponent( this );

    return false;
}

void PhysicsFrame::checkIfTeleport()
{
    const unsigned chId = userControlledList_.At(0);
    RefFrame * o = refFrame( chId );
    // Follow the first one.
    const Float teleportDist = Settings::teleportDistance();
    const Float d = o->distance();
    if ( d > teleportDist )
    {
        State st;
        o->relativeState( parent_id_, st );
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
        teleport( parent_id_, st );
    }
}

bool PhysicsFrame::checkIfNeedToSplit()
{
    const unsigned qty = userControlledList_.Size();
    if ( qty < 2 )
        return false;
    unsigned splitInd;
    const Float dist = cluster( this, splitInd, userControlledList_, userControlledList2_ );
    const Float splitDist = Settings::dynamicsWorldDistanceExclude();
    if ( dist < splitDist )
        return false;

    // Create another physics frame and move
    // second cluster nodes to that ref. frame.
    Scene * s = GetScene();
    PhysicsFrame * pf = s->CreateComponent<PhysicsFrame>();
    if ( parent_id_ > 0 )
        pf->setParent( parent_id_ );
    const unsigned pfId = pf->GetID();

    // Define the state of newly created physics frame.
    //{
    //    const State s = this->state();
    //    pf->setState( s );
    //}
    {
        const unsigned chId = userControlledList_[splitInd];
        RefFrame * o = refFrame( chId );
        State relativeSt;
        o->relativeState( parent_id_, relativeSt );
        relativeSt.q = relQ();
        relativeSt.v = relV();
        relativeSt.w = relW();
        pf->setState( relativeSt );
    }

    // Move user controlled objects to the new physics frame.
    for ( unsigned i=splitInd; i<qty; i++ )
    {
        const unsigned chId = userControlledList_[splitInd];
        RefFrame * o = refFrame( chId );
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
            const unsigned chId = userControlledList_[i];
            RefFrame * o = refFrame( chId );
            const bool userCtrled = o->getUserControlled();
            if ( userCtrled )
                continue;
            SurfaceCollisionMesh * scm = o->Cast<SurfaceCollisionMesh>();
            if ( scm )
                continue;
            const Float distanceA = o->distance();
            const Float distanceB = o->distance( pfId );
            if ( distanceB < distanceA )
                o->setParent( pf );
        }
    }

    return true;
}

bool PhysicsFrame::checkIfNeedToMerge()
{
    bool result = false;
    RefFrame * p = parent();
    const Float mergeDist = Settings::dynamicsWorldDistanceInclude();
    const Vector3d r = relR();
    Scene * s = GetScene();
    if ( p )
    {
        userControlledList_ = p->children_;
        unsigned qty = userControlledList_.Size();
        for ( unsigned i=0; i<qty; i++ )
        {
            const unsigned chId = userControlledList_[i];
            RefFrame * o = refFrame( chId );
            PhysicsFrame * pf = o->Cast<PhysicsFrame>();
            if ( !pf )
                continue;
            if ( pf == this )
                continue;
            const Float dist = pf->distance( r );
            if ( dist > mergeDist )
                continue;

            // Move all objects to this physics frame.
            // Except for the SurfaceCollisionMesh. That one 
            // needs to be removed.
            userControlledList2_ = pf->children_;
            const unsigned qty = userControlledList2_.Size();
            for ( unsigned j=0; j<qty; j++ )
            {
                const unsigned chId = userControlledList2_[j];
                RefFrame * o = refFrame( chId );
                SurfaceCollisionMesh * scm = o->Cast<SurfaceCollisionMesh>();
                if ( scm )
                {
                    scm->setParent( nullptr );
                    s->RemoveComponent( scm );
                    continue;
                }
                o->setParent( this );
            }
            pf->setParent( nullptr );
            s->RemoveComponent( pf );
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
            if ( pf->parent_id_ > 0 )
                continue;

            const Float dist = pf->distance( r );
            if ( dist > mergeDist )
                continue;

            // Move all objects to a different physics frame.
            userControlledList2_ = pf->children_;
            const unsigned qty = userControlledList2_.Size();
            const unsigned thisId = this->GetID();
            for ( unsigned j=0; j<qty; j++ )
            {
                const unsigned chId = userControlledList2_[j];
                RefFrame * o = refFrame( chId );
                o->setParent( thisId );
            }
            pf->Remove();
            result = true;
        }
    }

    return result;
}

static Float cluster( RefFrame * rf, unsigned & splitInd, Vector<unsigned> & src, Vector<unsigned> & dest )
{
    const unsigned qty = src.Size();
    splitInd = qty;
    bool modified;
    Float baseScore = clusterScore( rf, src, splitInd );
    do
    {
        // Reset number of modifications done.
        modified = false;

        for ( unsigned i=0; i<qty; i++ )
        {
            clusterSwap( rf, src, dest, splitInd, i );
            const Float score = clusterScore( rf, dest, splitInd );
            if ( score < baseScore )
            {
                src       = dest;
                baseScore = score;
                modified  = true;
                break;
            }
        }
    } while ( modified );

    const Float dist = clusterDist( rf, src, splitInd );
    return dist;
}

static Float clusterScore( RefFrame * rf, Vector<unsigned> & objs, unsigned splitInd )
{
    Float score = 0.0;
    {
        if ( splitInd > 1 )
        {
            const unsigned upperBound = splitInd-1;
            for ( unsigned i=0; i<(splitInd-1); i++ )
            {
                const unsigned objIdI = objs[i];
                RefFrame * oi = rf->refFrame( objIdI );
                const Vector3d ri = oi->relR();
                const unsigned lowerBound = i + 1;
                for ( unsigned j=lowerBound; j<splitInd; j++ )
                {
                    const unsigned objIdJ = objs[j];
                    RefFrame * oj = rf->refFrame( objIdJ );
                    const Float dist = oj->distance( ri );
                    score += dist;
                }
            }
        }
    }

    {
        const unsigned qty = objs.Size();
        const unsigned upperBound = qty - 1;
        if ( splitInd < upperBound )
        {
            for ( unsigned i=splitInd; i<upperBound; i++ )
            {
                const unsigned objIdI = objs[i];
                RefFrame * oi = rf->refFrame( objIdI );
                const Vector3d ri = oi->relR();
                const unsigned lowerBound = i + 1;
                for ( unsigned j=lowerBound; j<qty; j++ )
                {
                    const unsigned objIdJ = objs[j];
                    RefFrame * oj = rf->refFrame( objIdJ );
                    const Float dist = oj->distance(ri);
                    score += dist;
                }
            }
        }
    }


    return score;
}


static Float clusterDist( RefFrame * rf, Vector<unsigned> & objs, unsigned splitInd )
{
    Float minDist = -1.0;
    const unsigned qty = objs.Size();
    for ( unsigned i=0; i<splitInd; i++ )
    {
        const unsigned objIdI = objs[i];
        RefFrame * oi = rf->refFrame( objIdI );
        const Vector3d ri = oi->relR();
        for ( unsigned j=splitInd; j<qty; j++ )
        {
            const unsigned objIdJ = objs[j];
            RefFrame * oj = rf->refFrame( objIdJ );
            const Vector3d rj = oj->relR();
            const Float distA = oj->distance( ri );
            const Float distB = oi->distance( rj );
            const Float dist = (distA + distB) * 0.5;
            if ( ( minDist < 0.0 ) || ( dist < minDist ) )
                minDist = dist;
        }
    }

    return minDist;
}

static void  clusterSwap( RefFrame * rf, Vector<unsigned> & src, Vector<unsigned> & dest, unsigned & splitInd, unsigned & elementInd )
{
    dest = src;
    if ( elementInd < splitInd )
    {
        const unsigned lastInd = splitInd - 1;
        if ( elementInd < lastInd )
        {
            unsigned a = dest[elementInd];
            unsigned b = dest[lastInd];
            unsigned tmp = b;
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
            unsigned a = dest[elementInd];
            unsigned b = dest[firstInd];
            unsigned tmp = b;
            dest[firstInd]   = a;
            dest[elementInd] = tmp;
        }
        splitInd += 1;
    }
}





}














