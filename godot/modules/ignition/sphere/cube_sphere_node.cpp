
#include "cube_sphere_node.h"
#include "ref_frame_node.h"

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

void CubeSphereNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_height_source", "height_source"), &CubeSphereNode::set_height_source );

	ClassDB::bind_method( D_METHOD("set_r", "r"), &CubeSphereNode::set_r );
	ClassDB::bind_method( D_METHOD("get_r" ),     &CubeSphereNode::get_r, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_h", "h"), &CubeSphereNode::set_h );
	ClassDB::bind_method( D_METHOD("get_h" ),     &CubeSphereNode::get_h, Variant::REAL );

	ClassDB::bind_method( D_METHOD("clear_levels"), &CubeSphereNode::clear_levels );
	ClassDB::bind_method( D_METHOD("add_level", "sz", "dist"), &CubeSphereNode::add_level );

	ClassDB::bind_method( D_METHOD("set_subdivision_check_period", "sec"), &CubeSphereNode::set_subdivision_check_period );
	ClassDB::bind_method( D_METHOD("get_subdivision_check_period"), &CubeSphereNode::get_subdivision_check_period, Variant::REAL );

	ClassDB::bind_method( D_METHOD("clear_points_of_interest"), &CubeSphereNode::clear_points_of_interest );
	ClassDB::bind_method( D_METHOD("add_point_of_interest", "at", "close"), &CubeSphereNode::add_point_of_interest );

	ClassDB::bind_method( D_METHOD("need_rebuild"), &CubeSphereNode::need_rebuild, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("rebuild"), &CubeSphereNode::rebuild );

	ClassDB::bind_method( D_METHOD("set_close", "en"), &CubeSphereNode::set_close );
	ClassDB::bind_method( D_METHOD("get_close"), &CubeSphereNode::get_close, Variant::BOOL );

	ClassDB::bind_method( D_METHOD("set_center_ref_frame", "path"), &CubeSphereNode::set_center_ref_frame );
	ClassDB::bind_method( D_METHOD("get_center_ref_frame"), &CubeSphereNode::get_center_ref_frame, Variant::NODE_PATH );

	ClassDB::bind_method( D_METHOD("set_origin_ref_frame", "path"), &CubeSphereNode::set_origin_ref_frame );
	ClassDB::bind_method( D_METHOD("get_origin_ref_frame"), &CubeSphereNode::get_origin_ref_frame, Variant::NODE_PATH );

	ClassDB::bind_method( D_METHOD("clear_ref_frames"), &CubeSphereNode::clear_ref_frames );
	ClassDB::bind_method( D_METHOD("add_ref_frame", "path"), &CubeSphereNode::add_ref_frame );
	ClassDB::bind_method( D_METHOD("remove_ref_frame", "path"), &CubeSphereNode::remove_ref_frame );


	ADD_PROPERTY( PropertyInfo( Variant::REAL, "radius" ), "set_r", "get_r" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "height" ), "set_h", "get_h" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "rebuild_check_period" ), "set_subdivision_check_period", "get_subdivision_check_period" );
	ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "center_ref_frame" ), "set_center_ref_frame", "get_center_ref_frame" );
	ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "origin_ref_frame" ), "set_origin_ref_frame", "get_origin_ref_frame" );
}

void CubeSphereNode::_notification( int p_what )
{
	switch ( p_what )
	{
	case NOTIFICATION_READY:
		init_levels();
		break;
	case NOTIFICATION_PROCESS:
		process_transform();
		break;
	default:
		break;
	}
}


CubeSphereNode::CubeSphereNode()
	: MeshInstance()
{
	height_source = nullptr;
	check_period   = 1.0;
	generate_close = true;

	set_process( true );
}

CubeSphereNode::~CubeSphereNode()
{
}

void CubeSphereNode::set_height_source( Ref<HeightSourceRef> hs )
{
	height_source = hs; //Object::cast_to<HeightSourceRef>( hs );
}


void CubeSphereNode::set_r( real_t r )
{
	sphere.set_r( r );
}

real_t CubeSphereNode::get_r() const
{
	return sphere.r();
}

void CubeSphereNode::set_h( real_t h )
{
	sphere.set_h( h );
}

real_t CubeSphereNode::get_h() const
{
	return sphere.h();
}

void CubeSphereNode::clear_levels()
{
	subdivide_source.clear_levels();
}

void CubeSphereNode::add_level( real_t sz, real_t dist )
{
	subdivide_source.add_level( sz, dist );
}

void CubeSphereNode::set_subdivision_check_period( real_t sec )
{
	check_period = sec;
}

real_t CubeSphereNode::get_subdivision_check_period() const
{
	return check_period;
}

void CubeSphereNode::clear_points_of_interest()
{
	points_of_interest.clear();
}

void CubeSphereNode::add_point_of_interest( const Vector3 & at, bool close )
{
	SubdivideSource::SubdividePoint pt;
	pt.at = Vector3d( at.x, at.y, at.z );
	pt.close = close;
	points_of_interest.push_back( pt );
}

bool CubeSphereNode::need_rebuild()
{
	const bool rebuild_is_needed = subdivide_source.need_subdivide( &sphere, points_of_interest );
	return rebuild_is_needed;
}

void CubeSphereNode::rebuild()
{
	sphere.subdivide( &subdivide_source );
	if ( height_source.ptr() != nullptr )
		sphere.apply_source( height_source->height_source );
	sphere.triangle_list( all_tris );

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
		const Vector3 at( v.at.x_, v.at.y_, v.at.z_ );
		const Vector3 n( v.norm.x_, v.norm.y_, v.norm.z_ );
		const Vector3 t   = compute_tangent( n );
		const Vector2 uv  = compute_uv( at, sphere.r() );
		const Vector2 uv2 = compute_uv2( at, sphere.r() );
		vertices.set( i, at );
		normals.set( i, n );
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

	this->set_mesh( am );
}

PoolVector3Array CubeSphereNode::triangles()
{
	PoolVector3Array arr;
	return arr;
}

PoolVector3Array CubeSphereNode::collision_triangles( real_t dist )
{
	PoolVector3Array arr;

	sphere.triangle_list( points_of_interest, dist, collision_tris );

	return arr;
}

void CubeSphereNode::set_close( bool en )
{
	generate_close = en;
}

bool CubeSphereNode::get_close() const
{
	return generate_close;
}

void CubeSphereNode::set_center_ref_frame( const NodePath & path )
{
	center_path = path;
	validate_ref_frames();
}

const NodePath & CubeSphereNode::get_center_ref_frame() const
{
	return center_path;
}

void CubeSphereNode::set_origin_ref_frame( const NodePath & path )
{
	origin_path = path;
	validate_ref_frames();
}

const NodePath & CubeSphereNode::get_origin_ref_frame() const
{
	return origin_path;
}

void CubeSphereNode::clear_ref_frames()
{
	ref_frame_paths.clear();
	validate_ref_frames();
}

void CubeSphereNode::add_ref_frame( const NodePath & path )
{
	const int ind = ref_frame_paths.find( path );
	if ( ind < 0 )
		ref_frame_paths.push_back( path );
	validate_ref_frames();
}

void CubeSphereNode::remove_ref_frame( const NodePath & path )
{
	ref_frame_paths.erase( path );
	validate_ref_frames();
}

void CubeSphereNode::validate_ref_frames()
{
	// Make sure the origin ref frame is in or not in depending on if it is valid.
	Node * n = get_node_or_null( origin_path );
	if ( n != nullptr )
	{
		const int ind = ref_frame_paths.find( origin_path );
		if ( ind < 0 )
			ref_frame_paths.push_back( origin_path );
	}
	else
	{
		ref_frame_paths.erase( origin_path );
	}

	// Go over all node paths and make sure they exist.
	ref_frames.clear();
	clear_points_of_interest();
	int current_index = 0;
	while ( current_index < ref_frame_paths.size() )
	{
		const NodePath & np = ref_frame_paths.ptr()[current_index];
		n = get_node_or_null( np );
		RefFrameNode * rfn = Object::cast_to<RefFrameNode>( n );
		if ( rfn == nullptr )
		{
			ref_frame_paths.remove( current_index );
			continue;
		}
		ref_frames.push_back( rfn );
		current_index += 1;
	}
}


void CubeSphereNode::process_transform()
{
	validate_ref_frames();

	Node * n = get_node_or_null( center_path );
	RefFrameNode * center_rf = Node::cast_to<RefFrameNode>( n );
	n = get_node_or_null( origin_path );
	RefFrameNode * origin_rf = Node::cast_to<RefFrameNode>( n );

	const int origin_index = ref_frames.find( origin_rf );
	const bool both_rf_ok = ( ( center_rf != nullptr ) &&
		                      ( origin_rf != nullptr ) &&
		                      ( origin_index >= 0 ) );

	if ( both_rf_ok )
	{
		const SE3 poi = origin_rf->relative_( center_rf );
		poi_relative_to_origin = center_rf->relative_( origin_rf );
	}
	else
		poi_relative_to_origin = SE3();

	// Adding points of interest.
	points_of_interest.clear();
	const int qty = ref_frames.size();
	for ( int i=0; i<qty; i++ )
	{
		RefFrameNode * rf = ref_frames.ptrw()[i];
		const SE3 se3 = rf->relative_( center_rf );
		SubdivideSource::SubdividePoint pt;
		pt.at    = se3.r_;
		pt.close = generate_close;
		points_of_interest.push_back( pt );
	}
	// Check if need to be rebuilt.
	const bool need_rebuild = this->need_rebuild();
	if ( !need_rebuild )
		return;

	const Float L    = poi_relative_to_origin.r_.Length();
	const Float _L_R = L - sphere.r();
	const Vector3d r = poi_relative_to_origin.r_ * (_L_R / L);
	SE3 se3 = poi_relative_to_origin;
	se3.r_ = r;
	poi_relative_to_center = se3 / poi_relative_to_origin;

	regenerate_mesh();
}

void CubeSphereNode::regenerate_mesh()
{
	sphere.subdivide( &subdivide_source );
	if ( height_source.ptr() != nullptr )
		sphere.apply_source( height_source->height_source );
	sphere.triangle_list( all_tris );

	// Inverted transform.
	const SE3 se3 = poi_relative_to_center.inverse();

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
		const Vector3d at = se3 * v.at;
		const Vector3d n  = se3.q_ * v.norm;
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

	this->set_mesh( am );
}

void CubeSphereNode::adjust_pose()
{
	const SE3 se3 = poi_relative_to_origin * poi_relative_to_center;
	// Here it should be distance scale.
	const Transform t = se3.transform();
	set_transform( t );
}

void CubeSphereNode::init_levels()
{
	if ( !Engine::get_singleton()->is_editor_hint() )
		return;

	const Float r = sphere.r();
	subdivide_source.add_level( r/20.0, r/5.0 );
	subdivide_source.add_level( r/5.0, r*3.0 );
}





}






