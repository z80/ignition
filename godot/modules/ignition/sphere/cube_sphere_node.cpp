
#include "cube_sphere_node.h"
#include "ref_frame_node.h"
#include "cube_quad_node.h"
#include "height_source.h"
#include "se3_ref.h"

#include "core/engine.h"

namespace Ign
{



static Vector3 compute_tangent( const Vector3 & n )
{
    const Float MIN_L = 0.1;
    {
        const Vector3 x = Vector3( 1.0, 0.0, 0.0 );
        Vector3 tx = x - (n * n.dot( x ));
        const real_t L = tx.length();
        if ( L >= MIN_L )
        {
            tx.normalize();
            return tx;
        }
    }
    {
        const Vector3 y = Vector3( 0.0, 1.0, 0.0 );
        Vector3 ty = y - (n * n.dot( y ));
        const real_t L = ty.length();
        if ( L >= MIN_L )
        {
            ty.normalize();
            return ty;
        }
    }
    const Vector3 z = Vector3( 0.0, 0.0, 1.0 );
    Vector3 tz = z - (n * n.dot( z ));
    const real_t L = tz.length();
    tz.normalize();
    return tz;
}

Vector2 compute_uv( const Vector3 & at, real_t R )
{
    const Vector2 uv( at.x / R, at.z / R );
    return uv;
}

Vector2 compute_uv2( const Vector3 & at, real_t R )
{
    const Vector2 uv( at.x / R, at.y / R );
    return uv;
}

static SE3 compute_poi_relative_to_center( CubeSphere * s, const SE3 & center_relative_to_rf )
{
	Float L = center_relative_to_rf.r_.Length();
	const Float R = s->r();
	if ( L < R )
		L = R;
	const Float _L_R = L - R;
	const Vector3d r = center_relative_to_rf.r_ * (_L_R / L);
	SE3 se3 = center_relative_to_rf;
	se3.r_ = r;
	const SE3 poi_relative_to_center = se3 / center_relative_to_rf;

	return poi_relative_to_center;
}

void CubeSphereNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_radius", "r"), &CubeSphereNode::set_radius );
	ClassDB::bind_method( D_METHOD("get_radius" ),     &CubeSphereNode::get_radius, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_height", "h"), &CubeSphereNode::set_height );
	ClassDB::bind_method( D_METHOD("get_height" ),     &CubeSphereNode::get_height, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_height_source", "height_source"), &CubeSphereNode::set_height_source );
    ClassDB::bind_method( D_METHOD("get_height_source"),                  &CubeSphereNode::get_height_source, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("clear_levels"), &CubeSphereNode::clear_levels);
	ClassDB::bind_method( D_METHOD("add_level", "sz", "dist"), &CubeSphereNode::add_level);

    ClassDB::bind_method( D_METHOD("collision_triangles", "ref_frame", "subdivide_source", "dist"), &CubeSphereNode::collision_triangles, Variant::POOL_VECTOR3_ARRAY );

    ClassDB::bind_method( D_METHOD("content_cells", "origin", "cell_size", "dist"), &CubeSphereNode::content_cells, Variant::ARRAY );
    ClassDB::bind_method( D_METHOD("local_se3", "cell_ind", "unit_at", "true_surface_normal"), &CubeSphereNode::local_se3, Variant::OBJECT );
    ClassDB::bind_method( D_METHOD("surface_se3", "unit_at", "height"), &CubeSphereNode::surface_se3, Variant::OBJECT );



    ClassDB::bind_method( D_METHOD("set_distance_scaler", "scaler"), &CubeSphereNode::set_distance_scaler );
    ClassDB::bind_method( D_METHOD("get_distance_scaler"), &CubeSphereNode::get_distance_scaler, Variant::OBJECT );

    ClassDB::bind_method( D_METHOD("set_apply_scale", "en"), &CubeSphereNode::set_apply_scale );
    ClassDB::bind_method( D_METHOD("get_apply_scale"), &CubeSphereNode::get_apply_scale, Variant::BOOL );

    ClassDB::bind_method( D_METHOD("set_scale_mode_distance", "radie"), &CubeSphereNode::set_scale_mode_distance );
    ClassDB::bind_method( D_METHOD("get_scale_mode_distance"), &CubeSphereNode::get_scale_mode_distance, Variant::REAL );

    ClassDB::bind_method( D_METHOD("set_convert_to_global", "en"), &CubeSphereNode::set_convert_to_global );
    ClassDB::bind_method( D_METHOD("get_convert_to_global"), &CubeSphereNode::get_convert_to_global, Variant::BOOL );



	ClassDB::bind_method( D_METHOD("relocate_mesh", "rf", "subdiv_src"), &CubeSphereNode::relocate_mesh );
	ClassDB::bind_method( D_METHOD("rebuild_shape", "rf", "subdiv_src"), &CubeSphereNode::rebuild_shape );
	ClassDB::bind_method( D_METHOD("apply_visual_mesh"), &CubeSphereNode::apply_visual_mesh );


	ClassDB::bind_method( D_METHOD("set_target_mesh", "path"), &CubeSphereNode::set_target_mesh);
	ClassDB::bind_method( D_METHOD("get_target_mesh"),         &CubeSphereNode::get_target_mesh, Variant::NODE_PATH);



    ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "height_source" ), "set_height_source", "get_height_source" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "radius" ), "set_radius", "get_radius" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "height" ), "set_height", "get_height" );
    ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "distance_scaler" ), "set_distance_scaler", "get_distance_scaler" );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "apply_scale" ), "set_apply_scale", "get_apply_scale" );
    ADD_PROPERTY( PropertyInfo( Variant::REAL, "scale_mode_distance" ), "set_scale_mode_distance", "get_scale_mode_distance" );
    ADD_PROPERTY( PropertyInfo( Variant::BOOL, "convert_to_global" ), "set_convert_to_global", "get_convert_to_global" );
	ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "target_mesh" ), "set_target_mesh", "get_target_mesh" );
}



CubeSphereNode::CubeSphereNode()
    : RefFrameNode()
{
    generate_close = true;
    apply_scale    = true;
    scale_mode_distance = 5.0;

    convert_to_global = false;

	distance_scale = 1.0;
}

CubeSphereNode::~CubeSphereNode()
{
}

void CubeSphereNode::set_height_source( const Ref<HeightSourceRef> & hs )
{
    height_source = hs;
}


Ref<HeightSourceRef> CubeSphereNode::get_height_source() const
{
    return height_source;
}

void CubeSphereNode::set_radius( real_t r )
{
    sphere.set_r( r );
}

real_t CubeSphereNode::get_radius() const
{
    return sphere.r();
}

void CubeSphereNode::set_height( real_t h )
{
    sphere.set_h( h );
}

real_t CubeSphereNode::get_height() const
{
    return sphere.h();
}

void CubeSphereNode::clear_levels()
{
    sphere.clear_levels();
}

void CubeSphereNode::add_level( real_t sz, real_t dist )
{
    sphere.add_level( sz, dist );
}

const PoolVector3Array & CubeSphereNode::collision_triangles( Node * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source_ref, real_t dist )
{
	RefFrameNode * rf = Node::cast_to<RefFrameNode>( ref_frame );
    PoolVector3Array & arr = collision_ret;
	if ( rf == nullptr )
	{
		arr.resize( 0 );
		return arr;
	}

    sphere.triangle_list( subdivide_source_ref->subdivide_points, dist, collision_tris );
    const SE3 center_rel_to_origin = this->relative_( rf );
    const int pts_qty = collision_tris.size();

    arr.resize( pts_qty );

	SE3 se3 = center_rel_to_origin;
	if ( convert_to_global )
	{
		SE3 to_global = rf->relative_( nullptr );
		se3 = to_global * se3;
	}

    // Convert to ref. frame relative to origin.
    for ( int i=0; i<pts_qty; i++ )
    {
        const CubeVertex & v = collision_tris.ptr()[i];
        Vector3d r = v.at;
        r = (se3.q_ * r) + se3.r_;
        Vector3 c( r.x_, r.y_, r.z_ );
        arr.set( i, c );
    }

    return arr;
}


const Array & CubeSphereNode::content_cells( Node * ref_frame, real_t cell_size, real_t dist )
{
	RefFrameNode * rf = Node::cast_to<RefFrameNode>( ref_frame );
    Array & arr = content_cells_ret;
	if ( rf == nullptr )
	{
		arr.resize( 0 );
		return arr;
	}

    content_pts.clear();
	// For converting to origin rf.
	const SE3 to_origin_se3 = rf->relative_( this );

	// Adding a single interest point for content.
    content_pts.push_back( to_origin_se3.r_ );


    // Traversing all the faces.
    sphere.face_list( content_pts, cell_size, dist, content_cell_inds );
    const int qty = content_cell_inds.size();
    arr.resize( qty );
    for ( int i=0; i<qty; i++ )
    {
        int counter = 0;
        Array cell_data;
        cell_data.resize( 7 );
        const int ind = content_cell_inds.ptr()[i];
        const CubeQuadNode & face = sphere.faces.ptr()[ind];
        const CubeVertex & v0 = sphere.verts.ptr()[ face.vertexInds[0] ];
        const CubeVertex & v1 = sphere.verts.ptr()[ face.vertexInds[1] ];
        const CubeVertex & v2 = sphere.verts.ptr()[ face.vertexInds[2] ];
        const CubeVertex & v3 = sphere.verts.ptr()[ face.vertexInds[3] ];
        // Face index.
        cell_data.set( counter++, Variant( ind ) );
        // Face hash.
        const uint64_t hash =  face.hash_.state();
        const String hash_s = String::num_uint64( hash );
        cell_data.set( counter++, Variant( hash_s ) );
        // Center height.
        const Float h = ( (v0.heightUnit_ + v1.heightUnit_ + v2.heightUnit_ + v3.heightUnit_) * sphere.h() ) / 4.0;
        cell_data.set( counter++, Variant( real_t(h) ) );
        // Deviation from vertical angle.
        const Vector3d c = (v0.atUnit + v1.atUnit + v2.atUnit + v3.atUnit) / 4.0;
        const Vector3d norm = (v0.norm + v1.norm + v2.norm + v3.norm) / 4.0;
        const Float angle = std::asin( c.CrossProduct( norm ).Length() );
        cell_data.set( counter++, angle );
        // Center unit vector.
        cell_data.set( counter++, Variant( Vector3( c.x_, c.y_, c.z_ ) ) );
        // 0-2 diagonal divided by sqrt(2).
        const Float d_02 = (v2.at - v0.at).Length() * 0.707;
        cell_data.set( counter++,  Variant( real_t(d_02) ) );
        // 1-3 diagonal divided by sqrt(2).
        const Float d_13 = (v3.at - v1.at).Length() * 0.707;
        cell_data.set( counter,  Variant( real_t(d_13) ) );

        arr.set( i, cell_data );
    }

    return arr;
}

Ref<Se3Ref> CubeSphereNode::local_se3( int cell_ind, const Vector2 & unit_at, bool true_surface_normal ) const
{
    const CubeQuadNode & face = sphere.faces.ptr()[cell_ind];
    const CubeVertex & v0 = sphere.verts.ptr()[ face.vertexInds[0] ];
    const CubeVertex & v1 = sphere.verts.ptr()[ face.vertexInds[1] ];
    const CubeVertex & v2 = sphere.verts.ptr()[ face.vertexInds[2] ];
    const CubeVertex & v3 = sphere.verts.ptr()[ face.vertexInds[3] ];

    const Vector3d ex   = v1.atUnit - v0.atUnit;
    const Vector3d ey_a = v2.atUnit - v0.atUnit;
    const Vector3d ey_b = v3.atUnit - v0.atUnit;

    const Float x = unit_at.x;
    const Float y = unit_at.y;
    const Float _1_x = 1.0 - x;
    const Float _1_y = 1.0 - y;

    Vector3d r_unit = ex*x + (ey_a*x + ey_b*_1_x)*y;
    r_unit.Normalize();
    Float h;
    if (height_source.ptr() == nullptr)
        h = 0.0;
    else
    {
        const HeightSourceRef * hsr = height_source.ptr();
        h = hsr->height_source->height( r_unit );
        const Float height_scale = sphere.h();
        h = h * height_scale;
    }
    const Vector3d r_scaled = r_unit * (sphere.r() + h);

    Ref<Se3Ref> se3;
    se3.instance();
    se3.ptr()->se3.r_ = r_scaled;

    if ( true_surface_normal )
    {
        const Vector3d na = (v0.norm * _1_x) + (v1.norm * x);
        const Vector3d nb = (v3.norm * _1_x) + (v2.norm * x);
        Vector3d norm = (na * _1_y) + (nb * y);
        norm.Normalize();
        const Quaterniond q( Vector3d(0.0, 1.0, 0.0 ), norm );
        se3.ptr()->se3.q_ = q;
    }
    else
    {
        const Quaterniond q( Vector3d(0.0, 1.0, 0.0 ), r_unit );
        se3.ptr()->se3.q_ = q;
    }

    return se3;
}

Ref<Se3Ref> CubeSphereNode::surface_se3( const Vector3 & unit_at, real_t height ) const
{
    Ref<Se3Ref> se3;
    se3.instance();

    Vector3d at = unit_at;
    at.Normalize();

    Float h;
    if (height_source.ptr() == nullptr)
        h = 0.0;
    else
    {
        const HeightSourceRef * hsr = height_source.ptr();
        h = hsr->height_source->height( at );
        const Float height_scale = sphere.h();
        h = h * height_scale;
    }
    const Quaterniond q( Vector3d( 0.0, 1.0, 0.0 ), unit_at );
    se3->se3.r_ = (sphere.r() + h + height) * at;
    se3->se3.q_ = q;

    return se3;
}

void CubeSphereNode::set_target_mesh( const NodePath & path )
{
    target_path = path;
}

const NodePath & CubeSphereNode::get_target_mesh() const
{
    return target_path;
}

void CubeSphereNode::set_distance_scaler( const Ref<DistanceScalerRef> & new_scaler )
{
    scale = new_scaler;
}

Ref<DistanceScalerRef> CubeSphereNode::get_distance_scaler() const
{
    return scale;
}

void CubeSphereNode::set_apply_scale( bool en )
{
    apply_scale = en;
}

bool CubeSphereNode::get_apply_scale() const
{
    return apply_scale;
}

void CubeSphereNode::set_scale_mode_distance( real_t radie )
{
    scale_mode_distance = radie;
}

real_t CubeSphereNode::get_scale_mode_distance() const
{
    return scale_mode_distance;
}

void CubeSphereNode::set_convert_to_global( bool en )
{
    convert_to_global = en;
}

bool CubeSphereNode::get_convert_to_global() const
{
    return convert_to_global;
}

void CubeSphereNode::relocate_mesh( Node * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source )
{
	RefFrameNode * rf = Node::cast_to<RefFrameNode>( ref_frame );
	adjust_pose( rf, subdivide_source );
}

void CubeSphereNode::rebuild_shape( Node * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source_ref )
{
	RefFrameNode * rf = Node::cast_to<RefFrameNode>( ref_frame );
	if ( rf == nullptr )
		return;
	regenerate_mesh( rf, subdivide_source_ref );
}

void CubeSphereNode::apply_visual_mesh()
{
	// Fill in arrays.
	const int qty = all_tris.size();
	vertices.resize( qty );
	normals.resize( qty );
	tangents.resize( qty*4 );
	colors.resize( qty );
	uvs.resize( qty );
	uvs2.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const CubeVertex & v = all_tris.ptr()[i];
		// Vertex position and normal are supposed to be relative to point of interest.
		// They are converted to that ref. frame in scaling methods.
		const Vector3d at = v.at;
		const Vector3d n  = v.norm;
		const Vector3 at_f( at.x_, at.y_, at.z_ );
		const Vector3 n_f( n.x_, n.y_, n.z_ );
		const Vector3 t   = compute_tangent( n_f );
		const Vector2 uv  = compute_uv( at_f, sphere.r() );
		const Vector2 uv2 = compute_uv2( at_f, sphere.r() );

		vertices.set( i, at_f );
		normals.set( i, n_f );
		int ind = i*4;
		tangents.set( ind++, t.x );
		tangents.set( ind++, t.y );
		tangents.set( ind++, t.z );
		tangents.set( ind, 1.0 );
		const Color c = v.color;
		colors.set( i, c );
		uvs.set( i, uv );
		uvs2.set( i, uv2 );
	}

	Array arrays;
	arrays.resize( ArrayMesh::ARRAY_MAX );
	arrays.set( ArrayMesh::ARRAY_VERTEX,  vertices );
	arrays.set( ArrayMesh::ARRAY_NORMAL,  normals );
	arrays.set( ArrayMesh::ARRAY_TANGENT, tangents );
	arrays.set( ArrayMesh::ARRAY_COLOR,   colors );
	arrays.set( ArrayMesh::ARRAY_TEX_UV,  uvs );
	arrays.set( ArrayMesh::ARRAY_TEX_UV2, uvs2 );

	Ref<ArrayMesh> am = memnew(ArrayMesh);
	am->add_surface_from_arrays( Mesh::PRIMITIVE_TRIANGLES, arrays );

	MeshInstance * mi = get_mesh_instance();
	mi->set_mesh( am );
}




void CubeSphereNode::regenerate_mesh( RefFrameNode * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source_ref )
{
	// Regenerate spherical landscape.
	SubdivideSource * ss = const_cast<SubdivideSource *>( &(subdivide_source_ref->subdivide_source) );
	sphere.subdivide( ss );
	if ( height_source.ptr() != nullptr )
		sphere.apply_source( height_source->height_source );
	else
		sphere.apply_source( nullptr );
	// Obtain triangles.
	sphere.triangle_list( all_tris );


	// Reference frame convertion.
	center_relative_to_ref_frame = this->relative_( ref_frame );

    // Compute point of interest relative to center.
    
    ss->poi_relative_to_center = compute_poi_relative_to_center( &sphere, center_relative_to_ref_frame );
    // "poi_relative_to_center" will be changed later in scale methods.

    // Check which way to scale the sphere.
    const bool use_scale = apply_scale ? (scale.ptr() != nullptr) : false;
    const Float d = center_relative_to_ref_frame.r_.Length() / sphere.r();
    const bool generate_close = ( d <= scale_mode_distance );

    // Do scale the sphere.
	// I.e. modify triangles.
    if ( use_scale )
    {
        if ( generate_close )
            scale_close( ss->poi_relative_to_center );
        else
            scale_far( ss->poi_relative_to_center );
    }
    else
        scale_neutral( ss->poi_relative_to_center );

}

void CubeSphereNode::adjust_pose( RefFrameNode * ref_frame, const Ref<SubdivideSourceRef> & subdivide_source )
{
	MeshInstance * mi = get_mesh_instance();
	if ( mi == nullptr )
		return;
	
	center_relative_to_ref_frame = this->relative_( ref_frame );
	// Apply scale.
	center_relative_to_ref_frame.r_ *= distance_scale;

	SubdivideSource * ss = const_cast<SubdivideSource *>( &(subdivide_source->subdivide_source) );
    SE3 se3 = center_relative_to_ref_frame * ss->poi_relative_to_center;
    if ( convert_to_global )
    {
        SE3 to_global = ref_frame->relative_( nullptr );
        se3 = to_global * se3;
    }
    // Here it should be distance scale.
    const Transform t = se3.transform();
    mi->set_transform( t );
}

void CubeSphereNode::init_levels()
{
    if ( !Engine::get_singleton()->is_editor_hint() )
        return;

    const Float r = sphere.r();
    add_level( r/20.0, r/5.0 );
    add_level( r/5.0, r*3.0 );
}

void CubeSphereNode::scale_close( SE3 & poi_relative_to_center )
{
    // Here assume that 1) scale is applied and 2) scaler object is set.
    // Convert to origin (observer) ref frame.
    const SE3 & center = center_relative_to_ref_frame;
    const int qty = all_tris.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = all_tris.ptrw()[i];
        Vector3d r = v.at;
        // Point relative to the observer.
        r = (center.q_ * r) + center.r_;
        const Float d = r.Length();
        const Float s = scale->scale( d );
        r = r * (s / d);
        v.at = r;
        v.norm = center.q_ * v.norm;
    }
    // Point of interest relative to origin.
    SE3 poi_to_origin = center_relative_to_ref_frame * poi_relative_to_center;
    const Float poi_d = poi_to_origin.r_.Length();
    const Float poi_s = scale->scale( poi_d ) / poi_d;
    poi_to_origin.r_ = poi_to_origin.r_ * poi_s;

    const SE3 origin_to_poi = poi_to_origin.inverse();

    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = all_tris.ptrw()[i];
        Vector3d r = v.at;
        // Point relative to poi.
        r = (origin_to_poi.q_ * r) + origin_to_poi.r_;
        v.at = r;
        // Rotate normal.
        v.norm = poi_to_origin.q_ * v.norm;
    }

    // Make POI relative to center such that scaled POI in origin rf
    // is obtained by center_relative_to_ref_frame * poi_relative_to_center.
	center_relative_to_ref_frame.r_ *= poi_s;
	poi_relative_to_center.r_       *= poi_s;
	distance_scale = poi_s;
}

void CubeSphereNode::scale_far( SE3 & poi_relative_to_center )
{
	// Make POI relative to center such that scaled POI in origin rf
	// is obtained by center_relative_to_ref_frame * poi_relative_to_center.
	SE3 poi_to_origin = center_relative_to_ref_frame * poi_relative_to_center;
	const Float poi_d = poi_to_origin.r_.Length();
	const Float poi_s = scale->scale( poi_d ) / poi_d;

	const SE3 center_to_poi = poi_relative_to_center.inverse();
    const int qty = all_tris.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = all_tris.ptrw()[i];
        Vector3d r = v.at;
        r = (center_to_poi.q_ * r) + center_to_poi.r_;
        r = r * poi_s;
        v.at = r;
        // Rotate normal.
        v.norm = center_to_poi.q_ * v.norm;
    }

	center_relative_to_ref_frame.r_ *= poi_s;
    poi_relative_to_center.r_       *= poi_s;
	distance_scale = poi_s;
}

void CubeSphereNode::scale_neutral( const SE3 & poi_relative_to_center )
{
    const SE3 inv_center = poi_relative_to_center.inverse();
    const int qty = all_tris.size();
    for ( int i=0; i<qty; i++ )
    {
        CubeVertex & v = all_tris.ptrw()[i];
        Vector3d r = v.at;
        r = ( inv_center.q_ * r) + inv_center.r_;
        v.at = r;
    }
}

MeshInstance * CubeSphereNode::get_mesh_instance()
{
	Node * node_mesh = get_node( target_path );
	if ( node_mesh == nullptr )
		return nullptr;
	MeshInstance * mi = Node::cast_to<MeshInstance>(node_mesh);
	return mi;
}







}






