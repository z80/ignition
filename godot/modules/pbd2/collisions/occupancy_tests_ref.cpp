
#include "occupancy_tests_ref.h"


namespace Pbd
{

void OccupancyTestsRef::_bind_methods()
{
    ClassDB::bind_method( D_METHOD( "set_ray_a", "a", "b" ), &OccupancyTestsRef::set_ray_a );
    ClassDB::bind_method( D_METHOD( "set_ray_b", "a", "b" ), &OccupancyTestsRef::set_ray_b );
    ClassDB::bind_method( D_METHOD( "intersect_rays" ),      &OccupancyTestsRef::intersect_rays, Variant::BOOL );
    ClassDB::bind_method( D_METHOD( "common_perpendicualr_a" ), &OccupancyTestsRef::common_perpendicualr_a, Variant::VECTOR3 );
    ClassDB::bind_method( D_METHOD( "common_perpendicualr_b" ), &OccupancyTestsRef::common_perpendicualr_b, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "set_cube", "c", "sz_x", "sz_y", "sz_z" ), &OccupancyTestsRef::set_cube );
    ClassDB::bind_method( D_METHOD( "apply_to_cube", "t" ), &OccupancyTestsRef::apply_to_cube );

    ClassDB::bind_method( D_METHOD( "set_face", "a", "b", "c" ), &OccupancyTestsRef::set_face );
    ClassDB::bind_method( D_METHOD( "apply_to_face", "t" ), &OccupancyTestsRef::apply_to_face );

    ClassDB::bind_method( D_METHOD( "intersect_face_with_cube" ), &OccupancyTestsRef::intersect_face_with_cube, Variant::BOOL );

    ClassDB::bind_method( D_METHOD( "set_cube_b", "c", "sz_x", "sz_y", "sz_z" ), &OccupancyTestsRef::set_cube_b );
    ClassDB::bind_method( D_METHOD( "apply_to_cube_b", "t" ), &OccupancyTestsRef::apply_to_cube_b );
    ClassDB::bind_method( D_METHOD( "intersect_cubes" ), &OccupancyTestsRef::intersect_cubes, Variant::BOOL );

    ClassDB::bind_method( D_METHOD( "set_face_b", "a", "b", "c" ), &OccupancyTestsRef::set_face_b );
    ClassDB::bind_method( D_METHOD( "apply_to_face_b", "t" ), &OccupancyTestsRef::apply_to_face_b );
    
    ClassDB::bind_method( D_METHOD( "intersect_faces" ), &OccupancyTestsRef::intersect_faces, Variant::INT );
    ClassDB::bind_method( D_METHOD( "face_intersection_point", "ind" ), &OccupancyTestsRef::face_intersection_point, Variant::VECTOR3 );
    ClassDB::bind_method( D_METHOD( "face_depth_point", "ind" ), &OccupancyTestsRef::face_depth_point, Variant::VECTOR3 );


    ClassDB::bind_method( D_METHOD( "tree_apply", "t" ), &OccupancyTestsRef::tree_apply );
    ClassDB::bind_method( D_METHOD( "tree_clear" ), &OccupancyTestsRef::tree_clear );
    ClassDB::bind_method( D_METHOD( "tree_add_triangle", "a", "b", "c" ), &OccupancyTestsRef::tree_add_triangle );
    ClassDB::bind_method( D_METHOD( "tree_set_level", "level" ), &OccupancyTestsRef::tree_set_level );
    ClassDB::bind_method( D_METHOD( "tree_subdivide" ), &OccupancyTestsRef::tree_subdivide );
    ClassDB::bind_method( D_METHOD( "tree_depth", "at" ), &OccupancyTestsRef::tree_depth, Variant::ARRAY );
    ClassDB::bind_method( D_METHOD( "tree_probe_depth", "at" ), &OccupancyTestsRef::tree_probe_depth, Variant::VECTOR3 );

    ClassDB::bind_method( D_METHOD( "lines_sdf_nodes" ),     &OccupancyTestsRef::lines_sdf_nodes,     Variant::POOL_VECTOR3_ARRAY );
    ClassDB::bind_method( D_METHOD( "lines_surface_pts" ),   &OccupancyTestsRef::lines_surface_pts,   Variant::POOL_VECTOR3_ARRAY );
    ClassDB::bind_method( D_METHOD( "lines_pts_nodes" ),     &OccupancyTestsRef::lines_pts_nodes,     Variant::POOL_VECTOR3_ARRAY );
    ClassDB::bind_method( D_METHOD( "lines_aligned_nodes" ), &OccupancyTestsRef::lines_aligned_nodes, Variant::POOL_VECTOR3_ARRAY );
}

OccupancyTestsRef::OccupancyTestsRef()
{
    face_intersects_cube_ = false;
    cubes_intersect_      = false;
}

OccupancyTestsRef::~OccupancyTestsRef()
{
}

void OccupancyTestsRef::set_ray_a( const Vector3 & a, const Vector3 & b )
{
    ray_a0_ = Vector3d( a.x, a.y, a.z );
    ray_a1_ = Vector3d( b.x, b.y, b.z );
}

void OccupancyTestsRef::set_ray_b( const Vector3 & a, const Vector3 & b )
{
    ray_b0_ = Vector3d( a.x, a.y, a.z );
    ray_b1_ = Vector3d( b.x, b.y, b.z );
}

bool OccupancyTestsRef::intersect_rays()
{
    const bool ok = common_perpendicular( ray_a0_, ray_a1_, ray_b0_, ray_b1_, common_perp_a_, common_perp_b_ );
    return ok;
}

Vector3 OccupancyTestsRef::common_perpendicualr_a() const
{
    const Vector3 v( common_perp_a_.x_, common_perp_a_.y_, common_perp_a_.z_ );
    return v;
}

Vector3 OccupancyTestsRef::common_perpendicualr_b() const
{
    const Vector3 v( common_perp_b_.x_, common_perp_b_.y_, common_perp_b_.z_ );
    return v;
}

void OccupancyTestsRef::set_cube( const Vector3 & c, real_t sz_x, real_t sz_y, real_t sz_z )
{
    cube_.init( Vector3d( c.x, c.y, c.z ), sz_x, sz_y, sz_z );
}

void OccupancyTestsRef::apply_to_cube( const Transform & t )
{
    const Quat q = Quat( t.basis );
    Pose se3;
    se3.q = Quaterniond( q.w, q.x, q.y, q.z );
    const Vector3 r = t.origin;
    se3.r = Vector3d( r.x, r.y, r.z );

    cube_.apply( se3 );
}

void OccupancyTestsRef::set_face( const Vector3 & a, const Vector3 & b, const Vector3 & c )
{
    face_.init( Vector3d( a.x, a.y, a.z ), Vector3d( b.x, b.y, b.z ), Vector3d( c.x, c.y, c.z ) );
}

void OccupancyTestsRef::apply_to_face( const Transform & t )
{
    const Quat q = Quat( t.basis );
    Pose se3;
    se3.q = Quaterniond( q.w, q.x, q.y, q.z );
    const Vector3 r = t.origin;
    se3.r = Vector3d( r.x, r.y, r.z );

    face_.apply( se3 );
}


bool OccupancyTestsRef::intersect_face_with_cube() const
{
    const bool ok = cube_.intersects( face_ );
    return ok;
}



void OccupancyTestsRef::set_cube_b( const Vector3 & c, real_t sz_x, real_t sz_y, real_t sz_z )
{
    cube_b_.init( Vector3d( c.x, c.y, c.z ), sz_x, sz_y, sz_z );
}

void OccupancyTestsRef::apply_to_cube_b( const Transform & t )
{
    const Quat q = Quat( t.basis );
    Pose se3;
    se3.q = Quaterniond( q.w, q.x, q.y, q.z );
    const Vector3 r = t.origin;
    se3.r = Vector3d( r.x, r.y, r.z );

    cube_b_.apply( se3 );
}

bool OccupancyTestsRef::intersect_cubes() const
{
    const bool ok = cube_.intersects( cube_b_ );
    return ok;
}




void OccupancyTestsRef::set_face_b( const Vector3 & a, const Vector3 & b, const Vector3 & c )
{
    face_b_.init( Vector3d( a.x, a.y, a.z ), Vector3d( b.x, b.y, b.z ), Vector3d( c.x, c.y, c.z ) );
}

void OccupancyTestsRef::apply_to_face_b( const Transform & t )
{
    const Quat q = Quat( t.basis );
    Pose se3;
    se3.q = Quaterniond( q.w, q.x, q.y, q.z );
    const Vector3 r = t.origin;
    se3.r = Vector3d( r.x, r.y, r.z );

    face_b_.apply( se3 );
}

int OccupancyTestsRef::intersect_faces()
{
    bool own_triangle[2];
    int  edge_index[2];
    const int qty = face_.intersects_2( face_b_, face_face_intersection_point_, face_face_intersection_depth_, own_triangle, edge_index );
    return qty;
}

Vector3 OccupancyTestsRef::face_intersection_point( int ind ) const
{
    const Vector3 ret = Vector3( face_face_intersection_point_[ind].x_, 
                                 face_face_intersection_point_[ind].y_, 
                                 face_face_intersection_point_[ind].z_ );
    return ret;
}

Vector3 OccupancyTestsRef::face_depth_point( int ind ) const
{
    const Vector3 ret = Vector3( face_face_intersection_depth_[ind].x_, 
                                 face_face_intersection_depth_[ind].y_, 
                                 face_face_intersection_depth_[ind].z_ );
    return ret;
}



void OccupancyTestsRef::tree_apply( const Transform & t )
{
    const Quat q = Quat( t.basis );
    Pose pose;
    pose.q = Quaterniond( q.w, q.x, q.y, q.z );
    const Vector3 r = t.origin;
    pose.r = Vector3d( r.x, r.y, r.z );

    tree_.pose = pose;
}

void OccupancyTestsRef::tree_clear()
{
    tree_.clear();
}

void OccupancyTestsRef::tree_add_triangle( const Vector3 & a, const Vector3 & b, const Vector3 & c )
{
    tree_.append_triangle( Vector3d( a.x, a.y, a.z ), Vector3d( b.x, b.y, b.z ), Vector3d( c.x, c.y, c.z ) );
}

void OccupancyTestsRef::tree_set_level( int level )
{
    tree_.set_max_depth( level );
}

void OccupancyTestsRef::tree_subdivide()
{
    tree_.subdivide();
}

Array OccupancyTestsRef::tree_depth( const Vector3 & at )
{
    Array ret;
    ret.resize( 3 );

    const Vector3d at_d( at.x, at.y, at.z );

    const NarrowTreeSdfNode & n = tree_.nodes_sdf_.ptr()[0];
    Float    d;
    Vector3d depth_d;
    const bool ok = n.distance( at_d, d, depth_d );
    const Vector3 depth( depth_d.x_, depth_d.y_, depth_d.z_ );
    ret.set( 0, ok );
    ret.set( 1, static_cast<real_t>( d ) );
    ret.set( 2, depth );

    return ret;
}

Vector3 OccupancyTestsRef::tree_probe_depth( const Vector3 & at )
{
    const NarrowTreeSdfNode & n = tree_.nodes_sdf_.ptr()[0];
    Vector3d depth;
    const Float d = n.probe_distance( at, depth );
    Vector3 ret( depth.x_, depth.y_, depth.z_ );
    return ret;
}


PoolVector3Array OccupancyTestsRef::lines_sdf_nodes() const
{
    PoolVector3Array ret = tree_.lines_sdf_nodes();
    return ret;
}

PoolVector3Array OccupancyTestsRef::lines_surface_pts() const
{
    PoolVector3Array ret = tree_.lines_surface_pts();
    return ret;
}

PoolVector3Array OccupancyTestsRef::lines_pts_nodes() const
{
    PoolVector3Array ret = tree_.lines_pts_nodes();
    return ret;
}

PoolVector3Array OccupancyTestsRef::lines_aligned_nodes() const
{
    PoolVector3Array ret = tree_.lines_aligned_nodes();
    return ret;
}











}











