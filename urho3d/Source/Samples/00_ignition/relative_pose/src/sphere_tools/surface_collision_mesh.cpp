

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

    CustomGeometry * cg = physics_node_->CreateComponent<CustomGeometry>();
    customGeometry_ = SharedPtr<CustomGeometry>( cg );
    // Make it invisible.
    cg->SetEnabled( false );

    CustomGeometry * vcg = visual_node_->CreateComponent<CustomGeometry>();
    visualCustomGeometry_ = SharedPtr<CustomGeometry>( vcg );

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    Material * m = cache->GetResource<Material>("Materials/VertexColor.xml");
    //Material * m = cache->GetResource<Material>("Materials/Stone.xml");
    vcg->SetMaterial( m );
    vcg->SetCastShadows( true );
}

void SurfaceCollisionMesh::createVisualContent( Node * n )
{
    if ( !n )
        return;

    //ResourceCache * cache = GetSubsystem<ResourceCache>();

    //StaticModel * model = n->CreateComponent<StaticModel>( LOCAL );
    //model->SetModel( cache->GetResource<Model>("Ign/Models/TestCube.mdl") );
    //model->SetMaterial( cache->GetResource<Material>("Ign/Materials/TestCubeM.xml") );
}

void SurfaceCollisionMesh::setupPhysicsContent( RigidBody2 * rb, CollisionShape2 * cs )
{
    rb->SetMass( 0.0 );
    // Here need to specify custom geometry based one later.
    //cs->SetBox( Vector3( 1.0, 1.0, 1.0 ) );
    //cs->SetCustomGImpactMesh(  );

    constructCustomGeometry();
}

SphereItem * SurfaceCollisionMesh::pickSphere()
{
    // Technically, SphereItem should be one of the children of parent's parent.
    // But for debugging this SurfaceCollisionMesh might be attached to 
    // other things. Due to that check more thoroughly.

    RefFrame * p = parent();
    if ( !p )
        return nullptr;
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
    if ( !si )
        return nullptr;

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

    return nullptr;
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

    if ( !customGeometry_ )
        return;

    const Float dist = Settings::dynamicsWorldDistanceExclude();
    pts_.Clear();
    pts_.Push( lastState_.r );
    tris_.Clear();
    si->cubesphereCollision_.triangleList( pts_, dist, tris_ );

    // Convert to local reference frame.
    const Quaterniond invQ = lastState_.q.Inverse();
    const unsigned qty = tris_.Size();
    for ( unsigned i=0; i<qty; i++ )
    {
        Vertex & v = tris_[i];
        v.at = invQ * (v.at - lastState_.r);
        v.norm = invQ * v.norm;
    }

    CustomGeometry * cg  = customGeometry_;
    CustomGeometry * vcg = visualCustomGeometry_;
    cg->Clear();
    vcg->Clear();
    cg->SetNumGeometries( 1 );
    vcg->SetNumGeometries( 1 );
    cg->BeginGeometry( 0, TRIANGLE_LIST );
    //vcg->BeginGeometry( 0, TRIANGLE_LIST );
    vcg->BeginGeometry( 0, LINE_LIST );

    for ( unsigned i=0; i<qty; i++ )
    {
        const Vertex & v = tris_[i];
        const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
        const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );
        const Color & c( v.color );
        cg->DefineVertex( at );
        cg->DefineColor( c );
        cg->DefineNormal( n );

        vcg->DefineVertex( at );
        vcg->DefineColor( c );
        vcg->DefineNormal( n );
    }

    cg->Commit();
    vcg->Commit();

    if ( collision_shape_ )
        collision_shape_->SetCustomGImpactMesh( cg );
}


















