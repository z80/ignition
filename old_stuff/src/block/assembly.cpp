
#include "assembly.h"
#include "name_generator.h"
//#include "Urho3D/Physics/RigidBody.h"
//#include "Urho3D/Physics/Constraint.h"
//#include "Urho3D/Physics/PhysicsWorld.h"
#include "rigid_body_2.h"
#include "constraint_2.h"
#include "physics_world_2.h"
#include "planet_base.h"
#include "game_data.h"
#include "world_mover.h"

#include "MyEvents.h"

namespace Osp
{

Assembly::Assembly( Context * c )
    : ItemBase( c )
{
    inAtmosphere = true;
    onSurface    = true;
}

Assembly::~Assembly()
{
    destroy();
}

Assembly * Assembly::create( Node * root, const Design & d )
{
    if ( !root )
        return nullptr;

    Node * node = root->CreateChild( NameGenerator::Next( "Assembly" ) );

    Assembly * a = node->CreateComponent<Assembly>();
    a->design = d;
    // Create blocks.
    {
        const size_t blocksQty = d.blocks.size();
        for ( size_t i=0; i<blocksQty; i++ )
        {
            const Design::Block & db = d.blocks[i];
            const String & typeName = db.typeName;
            Node * n = node->CreateChild( NameGenerator::Next( typeName ) );
            Component * c = n->CreateComponent( StringHash( typeName ) );
            Block * b = c->Cast<Block>();
            if ( !b )
                continue;
            b->setQ( db.q );
            b->setR( db.r );
            SharedPtr<Block> sb( b );
            a->blocks.Push( sb );
        }
    }

    return a;
}



void Assembly::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
    unsigned qty = blocks.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        SharedPtr<Block> b = blocks[i];
        b->DrawDebugGeometry( debug, depthTest );
    }
}

void Assembly::setR( const Vector3d & r )
{
    URHO3D_LOGWARNINGF( "Assembly::setR( %f, %f, %f )", r.x_, r.y_, r.z_ );
    ItemBase::setR( r );
}

void Assembly::Start()
{
    Node * n = GetNode();
    KeplerMover * m = n->CreateComponent<KeplerMover>();
    mover = SharedPtr<KeplerMover>( m );
    mover->active = false;

    Scene * s = GetScene();
    Component * c = s->GetComponent( StringHash( "WorldMover" ), true );
    if ( c )
    {
        ItemBase * w = c->Cast<ItemBase>();
        worldMover = SharedPtr<ItemBase>( w );
    }

    subscribeToEvents();
}

void Assembly::Update( float timeStep )
{
    // Apply forces.
    if ( inWorld )
    {
        computePlanetForces();
        convertPlanetForces();
        applyPlanetForces();
    }
}

void Assembly::PostUpdate( float timeStep )
{
    // Here recompute position and orientation.
    if ( mover->active )
        updatePoseInOrbit();
    else if ( inWorld )
        updatePoseInWorld();

    // Check if time to leave/enter world.
    if ( inWorld )
    {
        const bool leave = needLeaveWorld();
        if ( leave )
            fromWorld();
    }
    else
    {
        const bool enter = needEnterWorld();
        if ( enter )
            toWorld();
    }

    // Check if need to change planet orbiting around.
    if ( !inWorld )
    {
        checkInfluence();
    }

    // Draw debug geometry.
//    Scene * s = GetScene();
//    DebugRenderer * debug = s->GetOrCreateComponent<DebugRenderer>();
//    if ( debug )
//        DrawDebugGeometry( debug, false );
}

void Assembly::destroy()
{
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( b )
                b->Remove();
        }
    }

    {
        const size_t qty = joints.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Constraint2 * c = joints[i];
            if ( c )
                c->Remove();
        }
    }
}

void Assembly::updatePoseInWorld()
{
    if ( !inWorld )
        return;

    // Retrieve all the blocks.
    Vector3d r( Vector3d::ZERO );
    Vector3d v( Vector3d::ZERO );
    const size_t qty = blocks.Size();
    for ( size_t i=0; i<qty; i++ )
    {
        SharedPtr<Block> sb = blocks[i];
        if ( !sb )
        {
            URHO3D_LOGERROR( "Assembly::updatePoseInWorld() one of blocks doesn\'t exist!" );
            continue;
        }
        const Vector3d ri = sb->relR();
        const Vector3d vi = sb->relV();
        r += ri;
        v += vi;
    }
    const float _1_qty = 1.0/static_cast<float>( qty );
    r = r * _1_qty;
    v = v * _1_qty;
    //Node * n = GetNode();
    //n->SetPosition( Vector3( r.x_, r.y_, r.z_ ) );

    setR( r );
    setV( v );
}

void Assembly::updatePoseInOrbit()
{
    if ( !mover )
        return;
    const Vector3d r = mover->relR();
    const Vector3d v = mover->relV();

    setR( r );
    setV( v );
}

void Assembly::checkInfluence()
{
    if ( inAtmosphere || onSurface )
        return;

    PlanetBase * closestP = planetOfInfluence();
    if ( planet == closestP )
        return;

    // Need to switch parent planet.
    // Compute relative pose and velocity.
    Vector3d rel_r, rel_v, rel_w;
    Quaterniond rel_q;
    relativeAll( closestP->mover, rel_r, rel_q, rel_v, rel_w );
    setParent( closestP->mover );
    planet = SharedPtr<PlanetBase>( closestP );
    setR( rel_r );
    setV( rel_v );
    setQ( rel_q );
    setW( rel_w );
    mover->launch( rel_v );
}

PlanetBase * Assembly::planetOfInfluence()
{
    if ( !gameData )
        return nullptr;

    const unsigned qty = gameData->planets.Size();
    Float maxF = -1.0;
    PlanetBase * closestP = nullptr;
    for ( unsigned i=0; i<qty; i++ )
    {
        Vector3d rel_r;
        Quaterniond rel_q;
        PlanetBase * p = gameData->planets[i];
        const bool relOk = relativePose( p, rel_r, rel_q );
        if ( !relOk )
            continue;
        const Float r = rel_r.Length();
        const Float GM = p->GM();
        const Float F = GM/(r*r);
        if ( (!closestP) || (F > maxF) )
        {
            maxF = F;
            closestP = p;
        }
    }

    return closestP;
}

bool Assembly::needLeaveWorld()
{
    if ( (!inWorld) )
        return false;

    // Current selected assembly can not leave dynamics world.
    WorldMover * wm =  worldMover->Cast<WorldMover>();
    if ( wm->assembly == this )
        return false;

    Vector3d rel_r;
    Quaterniond rel_q;
    relativePose( worldMover, rel_r, rel_q );
    const Float d = rel_r.Length();
    return ( d > GameData::DIST_WORLD_LEAVE );
}

bool Assembly::needEnterWorld()
{
    if ( inWorld )
        return false;

    Vector3d rel_r;
    Quaterniond rel_q;
    relativePose( worldMover, rel_r, rel_q );
    const Float d = rel_r.Length();
    return ( d < GameData::DIST_WORLD_ENTER );
}

void Assembly::toWorld()
{
    // Need some relative velocity.
    // For now just zero.
    const Vector3d v = Vector3d::ZERO;

    // Traverse all blocks and joints and add those to world.
    {
        const size_t qty = blocks.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            Block * b = blocks[i];
            if ( !b )
                continue;
            b->setParent( worldMover );
            b->toWorld();
            // Assign one and the same velocity to all the blocks.
            b->setV( v );
        }
    }
    {
        // Create joints.
        Design & d = design;
        const size_t connsQty = d.joints.size();
        for ( size_t i=0; i<connsQty; i++ )
        {
            const Design::Joint & dj = d.joints[i];
            const int indA = dj.blockA;
            const int indB = dj.blockB;

            SharedPtr<Block> blockA = this->blocks[indA];
            SharedPtr<Block> blockB = this->blocks[indB];

            // For now place joint point in the middle.
            Node * node = blockA->GetNode();
            Constraint2 * c = node->CreateComponent<Constraint2>();
            c->SetConstraintType( CONSTRAINT_HINGE_2 );
            c->SetDisableCollision( true );
            c->SetOtherBody( blockB->rigidBody() );
            //c->SetWorldPosition( Vector3( r.x_, r.y_, r.z_ ) );

            const Vector3 axisA( 0.0, 1.0, 0.0 );
            const Vector2 lim( 0.0, 0.0 );

            // Constraint position in first body's ref. frame.
            PivotMarker * pmA = blockA->pivots[ dj.slotA ];
            const Vector3d Ra = pmA->relR();
            Quaterniond Qa = d.blocks[indA].q;
            // Compute axis in other body's ref. frame.
            PivotMarker * pmB = blockB->pivots[ dj.slotB ];
            const Vector3d Rb = pmB->relR();
            Quaterniond Qb = d.blocks[indB].q;

            c->SetPosition( Vector3( Ra.x_, Ra.y_, Ra.z_ ) );
            c->SetOtherPosition( Vector3( Rb.x_, Rb.y_, Rb.z_ ) );
            c->SetRotation( Quaternion( Qa.w_, Qa.x_, Qa.y_, Qa.z_ ) );
            c->SetOtherRotation( Quaternion( Qb.w_, Qb.x_, Qb.y_, Qb.z_ ) );
            c->SetHighLimit( lim );
            c->SetLowLimit( lim );

            this->joints.Push( SharedPtr<Constraint2>( c ) );
        }
    }

    // Assign it's parent to be dynamics world.
    setParent( worldMover );

    WorldMover * wm = worldMover->Cast<WorldMover>();

    inWorld      = true;
    inAtmosphere = !wm->active;
}

void Assembly::fromWorld()
{
    // Computing average velocity.
    Vector3d mv = Vector3d::ZERO;
    // Traverse all blocks and joints and remove from world.
    const size_t bqty = blocks.Size();
    {
        for ( size_t i=0; i<bqty; i++ )
        {
            Block * b = blocks[i];
            if ( !b )
                continue;
            // Remove block from world. It should destroy
            // all dynamics related objects (rigid body and collision shape).
            v += b->relV();
            b->fromWorld();
            b->setParent( this );
        }
    }
    mv = mv * ( 1.0 / (Float)bqty );
    {
        const size_t jqty = joints.Size();
        for ( size_t i=0; i<jqty; i++ )
        {
            Constraint2 * c = joints[i];
            if ( !c )
                continue;
            c->Remove();
        }
        joints.Clear();
    }

    inWorld = false;

    // Assign planet.
    WorldMover * wm = worldMover->Cast<WorldMover>();
    PlanetBase * planet = wm->planet;
    planet = SharedPtr<PlanetBase>( planet );

    // If in atmosphere just change parent to planet's "dynamicsNode".
    const bool moverInAtmosphere = !wm->active;
    if ( moverInAtmosphere )
    {
        setParent( planet->dynamicsNode );
        inAtmosphere = true;
        onSurface    = true;
        return;
    }

    inAtmosphere = false;
    onSurface    = false;

    // Flying in orbit.
    const Vector3d v = wm->relV() + mv;
    setParent( planet->mover );
    wm->launch( v, planet->GM() );
}

void Assembly::computePlanetForces()
{
    if ( !inWorld )
        return;

    PlanetForces * f = planet->forces;
    const unsigned qty = blocks.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Block * b = blocks[i];
        if ( !b )
            continue;
        if ( inAtmosphere )
            f->applyClose( b );
        else
            f->applyFar( b );
    }
}

void Assembly::convertPlanetForces()
{
    if ( !worldMover )
        return;
    const unsigned qty = blocks.Size();
    Vector3d    rel_r;
    Quaterniond rel_q;
    for ( unsigned i=0; i<qty; i++ )
    {
        Block * b = blocks[i];
        if ( !b )
            continue;
        b->relativePose( worldMover, rel_r, rel_q );
        const Vector3d f = rel_q * b->gravity.F;
        b->gravity.fW = Vector3( f.x_, f.y_, f.z_ );
        // And also convert all friction forces.
        const unsigned f_qty = b->friction.Size();
        for ( unsigned j=0; j<f_qty; j++ )
        {
            ForceApplied & fa = b->friction[j];
            const Vector3d f = rel_q * fa.F;
            const Vector3d r = rel_q * fa.at;
            fa.fW  = Vector3( f.x_, f.y_, f.z_ );
            fa.atW = Vector3( r.x_, r.y_, r.z_ );
        }
    }
}

void Assembly::applyPlanetForces()
{
    const unsigned qty = blocks.Size();
    Vector3d    rel_r;
    Quaterniond rel_q;
    for ( unsigned i=0; i<qty; i++ )
    {
        Block * b = blocks[i];
        if ( !b )
            continue;
        RigidBody2 * rb = b->rigidBody();
        if ( !rb )
            continue;
        rb->ApplyForce( b->gravity.fW, b->gravity.atW );
        const unsigned f_qty = b->friction.Size();
        for ( unsigned j=0; j<f_qty; j++ )
        {
            const ForceApplied & fa = b->friction[j];
            rb->ApplyForce( fa.fW, fa.atW );
        }
    }
}

void Assembly::subscribeToEvents()
{
    SubscribeToEvent( MyEvents::E_ASSEMBLY_SELECTED, URHO3D_HANDLER( Assembly, OnAssemblySelected ) );
    SubscribeToEvent( MyEvents::E_WORLD_ADJUSTED,    URHO3D_HANDLER( Assembly, OnWorldAdjusted ) );
}

void Assembly::OnAssemblySelected( StringHash eventType, VariantMap & eventData )
{
    if ( !inWorld )
        return;
    WorldMover * wm = worldMover->Cast<WorldMover>();
    //if ( planet != wm->assembly->planet )
    if ( this != wm->assembly )
        fromWorld();
}

void Assembly::OnWorldAdjusted( StringHash eventType, VariantMap & eventData )
{
    if ( !inWorld )
        return;

    using namespace MyEvents::WorldAdjusted;
    const Variant & varR = eventData[ P_POS_ADJ ];
    const Vector3d dr = *(Vector3d *)(varR.GetVoidPtr());
    const Variant & varV = eventData[ P_VEL_ADJ ];
    const Vector3d dv = *(Vector3d *)(varV.GetVoidPtr());
    adjustMovementInWorld( dr, dv );
}

void Assembly::adjustMovementInWorld( const Vector3d & dr, const Vector3d & dv )
{
    const unsigned qty = blocks.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Block * b = blocks[i];
        const Vector3d r0 = b->relR();
        const Vector3d v0 = b->relV();
        const Vector3d r1 = r0 + dr;
        const Vector3d v1 = v0 + dv;

        b->setR( r1 );
        b->setV( v1 );
        RigidBody2 * rb = b->rigidBody();
        if ( rb )
        {
            const Vector3 vr1( r1.x_, r1.y_, r1.z_ );
            rb->SetPosition( vr1 );
            const Vector3 vv1( v1.x_, v1.y_, v1.z_ );
            rb->SetLinearVelocity( vv1 );
        }
    }
    // Adjust assembly coordinates based on block
    // coordinates.
    updatePoseInWorld();
}


}

