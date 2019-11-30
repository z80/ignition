
#include "physics_frame.h"
#include "settings.h"

namespace Ign
{


static Float cluster( Vector<SharedPtr<RefFrame> > & objs );
static Float clusterScore( Vector<SharedPtr<RefFrame> > & objs, unsigned startInd, unsigned endInd );
static Float clusterDist( Vector<SharedPtr<RefFrame> > & objs, unsigned splitInd );
static void  swap( Vector<SharedPtr<RefFrame> > & objs, unsigned & splitInd, unsigned & elementInd );


PhysicsFrame::PhysicsFrame( Context * context )
    : RefFrame( context )
{

}

PhysicsFrame::~PhysicsFrame()
{

}

void PhysicsFrame::physicsStep( float sec_dt )
{
    if ( !physicsWorld_ )
        return;
    physicsWorld_->Update( sec_dt );
}

void PhysicsFrame::childEntered( RefFrame * refFrame )
{

}

void PhysicsFrame::childLeft( RefFrame * refFrame )
{

}

void PhysicsFrame::OnSceneSet( Scene * scene )
{
    if ( !scene )
        return;

    env_ = scene->GetComponent<Environment>();
    if ( !env_ )
        return;

    const bool isServer = env_->IsServer();
    if ( !isServer )
        return;

    const String name( "Dynamics node" );
    Node * n = scene->CreateChild( name, LOCAL );
    node_         = SharedPtr<Node>( n );
    physicsWorld_ = n->CreateComponent<PhysicsWorld2>( LOCAL );
}

const Vector<SharedPtr<RefFrame> > & PhysicsFrame::userControlledObjects()
{
    // Select user controlled objects.
    userControlled_.Clear();
    const unsigned qty = children_.Size();
    for ( unsigned i=0 ;i<qty; i++ )
    {
        SharedPtr<RefFrame> o = children_[i];
        if ( o->getUserControlled() )
            userControlled_.Push( o );
    }
    return userControlled_;
}

void PhysicsFrame::checkInnerObjects( const Vector<SharedPtr<RefFrame> > & userControlled )
{
    if ( userControlled.Empty() )
        return;

    // For each object compute smallest distance to user controlled object.
    // If smallest distance is bigger than horizont distance exclude object from 
    // this ref. frame.
    const unsigned userQty = userControlled.Size();
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
            SharedPtr<RefFrame> userObj = userControlled[j];
            const Float dist = o->distance( userObj );
            if ( ( minDist < 0.0 ) || ( dist < minDist ) )
                minDist = dist;
        }

        if ( minDist > removeDist )
            o->setParent( parent_ );
    }
}

void PhysicsFrame::checkOuterObjects( const Vector<SharedPtr<RefFrame> > & userControlled )
{
    // At the moment don't know how to deal with it.
    
    

    // This is over-simplification. Need to redo this in future (!!!).

    // Right now just check all parent's children and check if need to include
    // either one.
    SharedPtr<RefFrame> p = parent_;
    if ( !p )
        return;
    const Vector<SharedPtr<RefFrame> > & objs = p->children_;
    unsigned qty = objs.Size();
    const Float includeDist = Settings::dynamicsWorldDistanceInclude();

    for ( usngined i=0; i<qty; i++ )
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
            o->setParent( this );
    }

}

void PhysicsFrame::checkIfWorthToExist( const Vector<SharedPtr<RefFrame> > & userControlled )
{
    // If there are no user objects remove this object.
    if ( !userControlled.Empty() )
        return;

    const unsigned qty = children_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<RefFrame> o = children_[i];
        o->setParent( parent_ );
    }

    // Remove this physics ref. frame.
    this->Remove();
}

void PhysicsFrame::checkIfTeleport( const Vector<SharedPtr<RefFrame>> & userControlled )
{
    SharedPtr<RefFrame> o = userControlled.At(0);
    // Follow the first one.
    const Float teleportDist = Settings::teleportDistance();
    const Float d = o->distance();
    if ( d > teleportDist )
    {
        const Vector3d & r = o->relR();
        teleport( parent_, r );
    }
}

void PhysicsFrame::checkIfNeedToSplit( const Vector<SharedPtr<RefFrame> > & userControlled )
{
    const unsigned qty = userControlled.Size();
    if ( qty < 2 )
        continue;
}

void PhysicsFrame::checkIfNeedToMerge()
{
    SharedPtr<RefFrame> p = parent_;
    if ( !p )
        return;
    const Float mergeDist = 
    const Vector3d r = relR();
    const Vector<SharedPtr<RefFrame> > & objs = p->children_;
    unsigned qty = objs.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<RefFrame> o = objs[i];
        PhysicsFrame * pf = o->Cast<PhysicsFrame>();
        if ( !pf )
            continue;
        const Float dist = o->distance( r );
        if ( dist <= mergeDist )
        {
            return;
        }
    }

}

static Float cluster( Vector<SharedPtr<RefFrame> > & objs )
{
    const unsigned qty = objs.Size();
    const unsigned boundaryInd = 1;
    unsigned modsQty = 1;
    while ( modsQty > 0 )
    {
        // Reset number of modifications done.
        modsQty = 0;
        const Float baseScore = clusterScore( objs, boundaryInd );

        // 1) Try move all items from A to B.
        const unsigned szA = boundaryInd;
        if ( szA > 1 )
        {
            objs.
        }

        // 2) Try move all items from B to A.
        const unsigned szB = qty - boundaryInd;
        if ( szB > 1 )
        {
        }
    }
}

static Float clusterScore( Vector<SharedPtr<RefFrame> > & objs, unsigned startInd, unsigned endInd )
{
    Float score = 0.0;
    for ( unsigned i=startInd; i<endInd; i++ )
    {
        SharedPtr<RefFrame> & oi = objs[i];
        const Vector3d ri = oi->relR();
        for ( unsigned j=startInd; j<endInd; j++ )
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

    return dist;
}

static void  swap( Vector<SharedPtr<RefFrame> > & objs, unsigned & splitInd, unsigned & elementInd )
{
    if ( elementInd < splitInd )
    {
    }
    else
    {
    }
}





}














