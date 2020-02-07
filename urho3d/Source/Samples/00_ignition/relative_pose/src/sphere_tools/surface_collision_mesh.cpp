

#include "surface_collision_mesh.h"
#include "sphere_item.h"
#include "settings.h"

void SurfaceCollisionMesh::RegisterComponent( Context * context )
{
    context->RegisterFactory<SurfaceCollisionMesh>();
    URHO3D_COPY_BASE_ATTRIBUTES( PhysicsItem );
}


SurfaceCollisionMesh::SurfaceCollisionMesh( Context * context )
    : PhysicsItem( context ),
      lastSphereItem_( nullptr )
{
    SubscribeToEvent( E_UPDATE, URHO3D_HANDLER(SurfaceCollisionMesh, Update) );
}

SurfaceCollisionMesh::~SurfaceCollisionMesh()
{
}

void SurfaceCollisionMesh::Update( StringHash eventType, VariantMap & eventData )
{
    (void)eventType;
    (void)eventData;

    const unsigned elapsed =  timer_.GetMSec( false );
    if ( elapsed > 5000 )
    {
        timer_.Reset();
        constructCustomGeometry();
    }
}

void SurfaceCollisionMesh::parentTeleported()
{
    // Recompute dynamic geometry.

    constructCustomGeometry();
}

bool SurfaceCollisionMesh::IsSelectable() const
{
    return false;
}

void SurfaceCollisionMesh::OnSceneSet( Scene * scene )
{
    PhysicsItem::OnSceneSet( scene );
    if ( !scene )
        return;
}

void SurfaceCollisionMesh::createVisualContent( Node * n )
{
    if ( !n )
        return;

    CustomGeometry * vcg = visual_node_->CreateComponent<CustomGeometry>();
    visualCustomGeometry_ = SharedPtr<CustomGeometry>( vcg );

    constructCustomGeometry();
}

void SurfaceCollisionMesh::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 0.0 );

    CustomGeometry * cg = physics_node_->CreateComponent<CustomGeometry>();
    customGeometry_ = SharedPtr<CustomGeometry>( cg );
    // Make it invisible.
    cg->SetEnabled( false );

    constructCustomGeometry();
}

static SphereItem * globalSphereItem( Scene * s )
{
    const Vector<SharedPtr<Component> > & comps = s->GetComponents();
    const unsigned compsQty = comps.Size();
    for ( unsigned i=0; i<compsQty; i++ )
    {
        Component * c = comps[i];
        if ( !c )
            continue;
        SphereItem * si = c->Cast<SphereItem>();
        if ( !si )
            continue;
        return si;
    }

    return nullptr;
}

SphereItem * SurfaceCollisionMesh::pickSphere()
{
    // Technically, SphereItem should be one of the children of parent's parent.
    // But for debugging this SurfaceCollisionMesh might be attached to 
    // other things. Due to that check more thoroughly.

    RefFrame * p = parent();
    if ( !p )
    {
        Scene * s = GetScene();
        if ( !s )
            return nullptr;

        return globalSphereItem( s );
    }
    SphereItem * si = p->Cast<SphereItem>();
    if ( si )
        return si;

    RefFrame * candidate = p->parent();
    if ( !candidate )
        return nullptr;
    si = p->Cast<SphereItem>();
    if ( si )
        return si;

    si = candidate->Cast<SphereItem>();
    if ( si )
        return si;

    const unsigned qty = candidate->children_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        RefFrame * c = children_[i];
        if ( !c )
            continue;
        si = c->Cast<SphereItem>();
        if ( si )
            return si;
    }

    // Here desperate to find a sphere.
    // Search for any sphere within a scene.
    Scene * s = GetScene();
    if ( !s )
        return nullptr;

    si = globalSphereItem( s );
    return si;
}

bool SurfaceCollisionMesh::needRebuild( SphereItem * & item )
{
    SphereItem * si = pickSphere();
    item = si;
    if ( lastSphereItem_ != si )
    {
        lastSphereItem_ = si;
        if ( si )
        {
            State s;
            relativeState( si, s );
            lastState_ = s;
        }
        else
            lastState_.r = Vector3d::ZERO;
        return true;
    }

    if ( !si )
        return false;

    State s;
    relativeState( si, s );
    const Vector3d d = s.r - lastState_.r;
    const Float dist = d.Length();

    const Float maxDist = Settings::dynamicsWorldDistanceInclude() / 3.0;
    if ( dist > maxDist )
    {
        lastState_ = s;
        return true;
    }

    return false;
}

void SurfaceCollisionMesh::constructCustomGeometry()
{
    SphereItem * si;
    const bool needRebuildOk = needRebuild( si );
    if ( !needRebuildOk )
        return;

    if ( customGeometry_ )
    {
        if ( collision_shape_ )
        {
            constructCustomGeometry( si, customGeometry_ );
            collision_shape_->SetCustomGImpactMesh( customGeometry_ );
        }
    }
    if ( visualCustomGeometry_ )
        constructCustomGeometry( si, visualCustomGeometry_ );
}

void SurfaceCollisionMesh::constructCustomGeometry( SphereItem * si, CustomGeometry * cg )
{
    if ( !cg )
        return;

    const Float dist = Settings::dynamicsWorldDistanceExclude();
    pts_.Clear();
    pts_.Push( lastState_.r );
    tris_.Clear();

    // For debugging pick triangles from visual, not collision sphere.
    //si->cubesphereCollision_.triangleList( pts_, dist, tris_ );
    si->cubesphereVisual_.triangleList( pts_, dist, tris_ );

    // Convert to local reference frame.
    const Quaterniond invQ = lastState_.q.Inverse();
    const unsigned qty = tris_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Vertex & v = tris_[i];
        v.at = invQ * (v.at - lastState_.r);
        v.norm = invQ * v.norm;
    }

    cg->Clear();
    cg->SetNumGeometries( 1 );
    cg->BeginGeometry( 0, TRIANGLE_LIST );

    for ( unsigned i=0; i<qty; i++ )
    {
        const Vertex & v = tris_[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );
        const Color & c( v.color );
        cg->DefineVertex( at );
        cg->DefineColor( c );
        cg->DefineNormal( -n );
    }

    cg->Commit();
}


















