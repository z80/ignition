
#include "marching_cubes_dual_gd.h"

#include "marching_cubes_dual_node.h"

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"


namespace Ign
{

void MarchingCubesDualGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_split_precision", "rel_diff"),             &MarchingCubesDualGd::set_split_precision );
	ClassDB::bind_method( D_METHOD("get_split_precision"),                         &MarchingCubesDualGd::get_split_precision, Variant::REAL );

	ClassDB::bind_method( D_METHOD("set_min_step", "step"),    &MarchingCubesDualGd::set_min_step );
	ClassDB::bind_method( D_METHOD("get_min_step"),            &MarchingCubesDualGd::get_min_step,  Variant::REAL );
	ClassDB::bind_method( D_METHOD("init_min_step", "volume"), &MarchingCubesDualGd::init_min_step, Variant::REAL );

	ClassDB::bind_method( D_METHOD("create_bounding_node", "contains_pt", "desired_size"), &MarchingCubesDualGd::create_bounding_node, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("subdivide_source_all", "volume", "strategy"),              &MarchingCubesDualGd::subdivide_source_all, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("subdivide_source", "bounding_node", "volume", "strategy"), &MarchingCubesDualGd::subdivide_source,     Variant::BOOL );

	ClassDB::bind_method( D_METHOD("query_close_nodes", "at", "dist", "max_size"), &MarchingCubesDualGd::query_close_nodes, Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("center_direction", "source_se3", "at" ),       &MarchingCubesDualGd::center_direction,  Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("get_tree_node", "ind"),                        &MarchingCubesDualGd::get_tree_node,     Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("se3_in_point", "at"),                                  &MarchingCubesDualGd::se3_in_point, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("asset_se3", "source_se3", "asset_at", "scaler"),       &MarchingCubesDualGd::asset_se3,    Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("asset_transform", "source_se3", "asset_at", "scaler"), &MarchingCubesDualGd::asset_transform,    Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("materials_used"),                                           &MarchingCubesDualGd::materials_used,   Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("apply_to_mesh", "source_se3", "central_point_se3", "material_ind", "mesh_instance", "scaler"), &MarchingCubesDualGd::apply_to_mesh );

	ClassDB::bind_method( D_METHOD("precompute_scaled_values", "source_se3", "central_point_se3", "material_index", "scaler"),     &MarchingCubesDualGd::precompute_scaled_values );
	ClassDB::bind_method( D_METHOD("apply_to_mesh_only", "mesh_instance"),                      &MarchingCubesDualGd::apply_to_mesh_only );
	ClassDB::bind_method( D_METHOD("apply_to_mesh_only_wireframe", "mesh_instance"),            &MarchingCubesDualGd::apply_to_mesh_only_wireframe );

	ClassDB::bind_method( D_METHOD("compute_source_se3", "source_se3", "pt_in_source_se3", "scaler"),               &MarchingCubesDualGd::compute_source_se3, Variant::TRANSFORM );
	ClassDB::bind_method( D_METHOD("compute_source_transform", "source_se3", "pt_in_source_se3", "scaler"),         &MarchingCubesDualGd::compute_source_transform, Variant::TRANSFORM );
	ClassDB::bind_method( D_METHOD("collision_faces", "source_se3", "dist"),                    &MarchingCubesDualGd::collision_faces, Variant::ARRAY );
	
	ClassDB::bind_method( D_METHOD("set_max_nodes_qty", "qty"),             &MarchingCubesDualGd::set_max_nodes_qty );
	ClassDB::bind_method( D_METHOD("get_max_nodes_qty"),                    &MarchingCubesDualGd::get_max_nodes_qty, Variant::INT );

	ClassDB::bind_method( D_METHOD("get_nodes_qty"),               &MarchingCubesDualGd::get_nodes_qty,   Variant::INT );
	ClassDB::bind_method( D_METHOD("get_node", "node_ind"),        &MarchingCubesDualGd::get_node,        Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("get_node_parent", "node_ind"), &MarchingCubesDualGd::get_node_parent, Variant::INT );

	ClassDB::bind_method( D_METHOD("get_dual_cells_qty"),          &MarchingCubesDualGd::get_dual_cells_qty, Variant::INT );
	ClassDB::bind_method( D_METHOD("get_dual_cell", "cell_ind"),   &MarchingCubesDualGd::get_dual_cell,      Variant::ARRAY );

	ADD_GROUP( "Ignition", "" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "min_step" ),        "set_min_step",        "get_min_step" );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "split_precision" ), "set_split_precision", "get_split_precision" );
	ADD_PROPERTY( PropertyInfo( Variant::INT, "max_nodes_qty" ) ,   "set_max_nodes_qty",   "get_max_nodes_qty" );
}

MarchingCubesDualGd::MarchingCubesDualGd()
	: Reference()
{
}

MarchingCubesDualGd::~MarchingCubesDualGd()
{
}

void MarchingCubesDualGd::set_split_precision( real_t rel_diff )
{
	cubes.set_split_precision( rel_diff );
}

real_t MarchingCubesDualGd::get_split_precision() const
{
	const real_t ret = cubes.get_split_precision();
	return ret;
}

void MarchingCubesDualGd::set_min_step( real_t step )
{
	cubes.set_min_step( step );
}

real_t MarchingCubesDualGd::get_min_step() const
{
	const real_t ret = cubes.get_min_step();
	return ret;
}

real_t MarchingCubesDualGd::init_min_step( const Ref<VolumeSourceGd> & volume )
{
	VolumeSource * src = volume.ptr()->source;
	const real_t ret = cubes.init_min_step( src );
	return ret;
}

Ref<BoundingNodeGd> MarchingCubesDualGd::create_bounding_node( const Ref<Se3Ref> & contains_pt, real_t desired_size ) const
{
	const SE3 & pt_se3 = contains_pt->se3;
	const Float sz     = desired_size;

	const MarchingCubesDualNode node = cubes.create_bounding_node( pt_se3.r_, sz );
	Ref<BoundingNodeGd> ret;
	ret.instance();
	ret->node  = node;

	return ret;
}

bool MarchingCubesDualGd::subdivide_source_all( const Ref<VolumeSourceGd> & volume, const Ref<VolumeNodeSizeStrategyGd> & strategy )
{
	VolumeSource   * volume_source   = volume.ptr()->source;
	const VolumeNodeSizeStrategyGd * strategy_gd_c = strategy.ptr();
	VolumeNodeSizeStrategyGd * strategy_gd = const_cast<VolumeNodeSizeStrategyGd *>(strategy_gd_c);
	VolumeNodeSizeStrategy   * rebuild_strategy = (strategy_gd != nullptr) ? (&(strategy_gd->strategy)) : nullptr;

	const bool ret = cubes.subdivide_source( volume_source, rebuild_strategy );
	return ret;
}

bool MarchingCubesDualGd::subdivide_source( const Ref<BoundingNodeGd> & bounding_node, const Ref<VolumeSourceGd> & volume, const Ref<VolumeNodeSizeStrategyGd> & strategy )
{
	const MarchingCubesDualNode & node = bounding_node.ptr()->node;

	VolumeSource   * volume_source   = volume.ptr()->source;

	VolumeNodeSizeStrategy   * rebuild_strategy;
	const VolumeNodeSizeStrategyGd * strategy_gd_c = strategy.ptr();
	if ( strategy_gd_c == nullptr )
	{
		rebuild_strategy = nullptr;
	}
	else
	{
		VolumeNodeSizeStrategyGd * strategy_gd = const_cast<VolumeNodeSizeStrategyGd *>(strategy_gd_c);
		rebuild_strategy = (strategy_gd != nullptr) ? (&(strategy_gd->strategy)) : nullptr;
	}

	const bool ret = cubes.subdivide_source( node, volume_source, rebuild_strategy );
	return ret;
}

Array MarchingCubesDualGd::query_close_nodes( const Vector3 & at_in_source, real_t dist, real_t max_size )
{
	ret_array.clear();
	const Vector3d at_d = Vector3d( at_in_source.x, at_in_source.y, at_in_source.z );
	const std::vector<int> indices = cubes.query_close_nodes( at_d, dist, max_size );
	const int qty = indices.size();
	for ( int i=0; i<qty; i++ )
	{
		const int ind = indices[i];
		ret_array.push_back( ind );
	}

	return ret_array;
}

Vector3 MarchingCubesDualGd::center_direction( const Ref<Se3Ref> & src_se3, const Vector3 & at ) const
{
	const SE3 & source_se3 = src_se3->se3;
	const Vector3d c = cubes.center_direction( source_se3, Vector3d( at.x, at.y, at.z ) );
	const Vector3 ret( c.x_, c.y_, c.z_ );
	return ret;
}

Ref<MarchingCubesDualNodeGd> MarchingCubesDualGd::get_tree_node( int ind )
{
	Ref<MarchingCubesDualNodeGd> ret;
	MarchingCubesDualNode * node = cubes.get_tree_node( ind );
	if ( node == nullptr )
	{
		return ret;
	}

	ret.instance();
	ret->cubes = &cubes;
	ret->node  = node;

	return ret;
}

Ref<Se3Ref> MarchingCubesDualGd::se3_in_point( const Vector3 & at, const Ref<Se3Ref> & inv_src_se3 ) const
{
	Ref<Se3Ref> se3;
	se3.instance();
	const Vector3d at_d( at.x, at.y, at.z );
	se3->se3 = cubes.se3_in_point( at_d );
	return se3;
}

Ref<Se3Ref> MarchingCubesDualGd::asset_se3( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & asset_at, const Ref<DistanceScalerBaseRef> & scaler ) const
{
	Ref<Se3Ref> ret_se3;
	ret_se3.instance();

	const SE3 & source_se3 = src_se3->se3;
	const SE3 & se3        = asset_at->se3;

	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	ret_se3->se3 = cubes.asset_se3( source_se3, se3, s );
	return ret_se3;
}

Transform MarchingCubesDualGd::asset_transform( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & asset_at, const Ref<DistanceScalerBaseRef> & scaler ) const
{
	const SE3 & source_se3 = src_se3->se3;
	const SE3 & se3        = asset_at->se3;

	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	const SE3 ret_se3 = cubes.asset_se3( source_se3, se3, s );
	const Transform t = ret_se3.transform();
	return t;
}


Array MarchingCubesDualGd::materials_used()
{
	const std::set<int> mats = cubes.materials();
	int qty = 0;
	for ( std::set<int>::const_iterator it=mats.begin(); it!=mats.end(); it++ )
	{
		qty += 1;
	}
	ret_array.resize( qty );
	int ind = 0;
	for ( std::set<int>::const_iterator it=mats.begin(); it!=mats.end(); it++ )
	{
		const int mat = *it;
		ret_array.set( ind, mat );
		ind += 1;
	}
	return ret_array;
}


void MarchingCubesDualGd::apply_to_mesh( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & central_point_se3, int material_index, Node * mesh_instance, const Ref<DistanceScalerBaseRef> & scaler )
{
	const SE3 & source_se3 = src_se3->se3;
	const Vector3d & central_point = central_point_se3->se3.r_;
	MeshInstance * mi = Object::cast_to<MeshInstance>(mesh_instance);
	if (mi == nullptr)
	{
		print_line( String( "ERROR: expects mesh instance as an argument, got something else." ) );
		return;
	}

	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	const std::vector<Vector3> & verts = cubes.vertices( source_se3, central_point, material_index, s );
	const std::vector<Vector3> & norms = cubes.normals( material_index );
	const std::vector<real_t> & tangs = cubes.tangents( material_index );
	const std::vector<Vector2> * p_uvs;
	const std::vector<Vector2> * p_uv2s;
	cubes.uvs( central_point, material_index, p_uvs, p_uv2s );

	const int verts_qty = verts.size();
	// Fill in arrays.
	vertices.resize( verts_qty );
	normals.resize( verts_qty );
	tangents.resize( verts_qty*4 );
	//colors.resize( verts_qty );
	uvs.resize( verts_qty );
	uv2s.resize( verts_qty );

	int vert_ind = 0;
	int tang_ind = 0;
	for ( int i=0; i<verts_qty; i++ )
	{
		const Vector3 & vert = verts[i];
		const Vector3 & norm = norms[i];
		vertices.set( i, vert );
		normals.set( i, norm );

		const Vector2 & uv  = (*p_uvs)[i];
		const Vector2 & uv2 = (*p_uv2s)[i];
		uvs.set( i, uv );
		uv2s.set( i, uv2 );
	}

	const int tangs_qty = tangs.size();
	for ( int i=0; i<tangs_qty; i++ )
	{
		const real_t tang = tangs[i];
		tangents.set( i, tang );
	}
	//const Color c = v.color;
	//colors.set( i, c );
	//uvs.set( i, uv );
	//uvs2.set( i, uv2 );

	Array arrays;
	arrays.resize( ArrayMesh::ARRAY_MAX );
	arrays.set( ArrayMesh::ARRAY_VERTEX,  vertices );
	arrays.set( ArrayMesh::ARRAY_NORMAL,  normals );
	arrays.set( ArrayMesh::ARRAY_TANGENT, tangents );
	//arrays.set( ArrayMesh::ARRAY_COLOR,   colors );
	arrays.set( ArrayMesh::ARRAY_TEX_UV,  uvs );
	arrays.set( ArrayMesh::ARRAY_TEX_UV2, uv2s );

	Ref<ArrayMesh> am = memnew(ArrayMesh);
	am->add_surface_from_arrays( Mesh::PRIMITIVE_TRIANGLES, arrays );

	mi->set_mesh( am );
}

void MarchingCubesDualGd::precompute_scaled_values( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & central_point_se3, int material_index, const Ref<DistanceScalerBaseRef> & scaler )
{
	const SE3 & source_se3 = src_se3->se3;
	const Vector3d & central_point = central_point_se3->se3.r_;

	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	cubes.precompute_scaled_values( source_se3, central_point, material_index, s );
}

void MarchingCubesDualGd::apply_to_mesh_only( Node * mesh_instance )
{
	MeshInstance * mi = Object::cast_to<MeshInstance>(mesh_instance);
	if (mi == nullptr)
	{
		print_line( String( "ERROR: expects mesh instance as an argument, got something else." ) );
		return;
	}

	const std::vector<Vector3> & verts = cubes.vertices();
	const std::vector<Vector3> & norms = cubes.normals();
	const std::vector<real_t> & tangs  = cubes.tangents();
	const std::vector<Vector2> & uv_s   = cubes.uvs();
	const std::vector<Vector2> & uv2_s  = cubes.uv2s();

	const int verts_qty = verts.size();
	// Fill in arrays.
	vertices.resize( verts_qty );
	normals.resize( verts_qty );
	tangents.resize( verts_qty*4 );
	//colors.resize( verts_qty );
	uvs.resize( verts_qty );
	uv2s.resize( verts_qty );

	int vert_ind = 0;
	int tang_ind = 0;
	for ( int i=0; i<verts_qty; i++ )
	{
		const Vector3 & vert = verts[i];
		const Vector3 & norm = norms[i];
		vertices.set( i, vert );
		normals.set( i, norm );

		const Vector2 & uv  = uv_s[i];
		const Vector2 & uv2 = uv2_s[i];
		uvs.set( i, uv );
		uv2s.set( i, uv2 );
	}

	const int tangs_qty = tangs.size();
	for ( int i=0; i<tangs_qty; i++ )
	{
		const real_t tang = tangs[i];
		tangents.set( i, tang );
	}
	//const Color c = v.color;
	//colors.set( i, c );
	//uvs.set( i, uv );
	//uvs2.set( i, uv2 );

	Array arrays;
	arrays.resize( ArrayMesh::ARRAY_MAX );
	arrays.set( ArrayMesh::ARRAY_VERTEX,  vertices );
	arrays.set( ArrayMesh::ARRAY_NORMAL,  normals );
	arrays.set( ArrayMesh::ARRAY_TANGENT, tangents );
	//arrays.set( ArrayMesh::ARRAY_COLOR,   colors );
	arrays.set( ArrayMesh::ARRAY_TEX_UV,  uvs );
	arrays.set( ArrayMesh::ARRAY_TEX_UV2, uv2s );

	Ref<ArrayMesh> am = memnew(ArrayMesh);
	am->add_surface_from_arrays( Mesh::PRIMITIVE_TRIANGLES, arrays );
	//am->add_surface_from_arrays( Mesh::PRIMITIVE_LINE_LOOP, arrays );
	//am->add_surface_from_arrays( Mesh::PRIMITIVE_LINE_STRIP, arrays );
	//am->add_surface_from_arrays( Mesh::PRIMITIVE_LINES, arrays );

	mi->set_mesh( am );
}

void MarchingCubesDualGd::apply_to_mesh_only_wireframe( Node * mesh_instance )
{
	MeshInstance * mi = Object::cast_to<MeshInstance>(mesh_instance);
	if (mi == nullptr)
	{
		print_line( String( "ERROR: expects mesh instance as an argument, got something else." ) );
		return;
	}

	const std::vector<Vector3> & verts = cubes.vertices();
	const std::vector<Vector3> & norms = cubes.normals();
	const std::vector<real_t> & tangs  = cubes.tangents();
	const std::vector<Vector2> & uv_s   = cubes.uvs();
	const std::vector<Vector2> & uv2_s  = cubes.uv2s();

	const int verts_qty = verts.size();

	// Fill in arrays.
	vertices.resize( verts_qty*2 );
	normals.resize( verts_qty*2 );
	uvs.resize( verts_qty*2 );
	uv2s.resize( verts_qty*2 );

	int vert_ind = 0;
	int tang_ind = 0;
	for ( int i=0; i<verts_qty; i+=3 )
	{
		const Vector3 & vert_a = verts[i];
		const Vector3 & vert_b = verts[i+1];
		const Vector3 & vert_c = verts[i+2];
		vertices.set( 2*i,   vert_a );
		vertices.set( 2*i+1, vert_b );
		vertices.set( 2*i+2, vert_b );
		vertices.set( 2*i+3, vert_c );
		vertices.set( 2*i+4, vert_c );
		vertices.set( 2*i+5, vert_a );

		const Vector3 & norm_a = norms[i];
		const Vector3 & norm_b = norms[i+1];
		const Vector3 & norm_c = norms[i+2];
		normals.set( 2*i,   norm_a );
		normals.set( 2*i+1, norm_b );
		normals.set( 2*i+2, norm_b );
		normals.set( 2*i+3, norm_c );
		normals.set( 2*i+4, norm_c );
		normals.set( 2*i+5, norm_a );

		const Vector2 & uv_a  = uv_s[i];
		const Vector2 & uv_b  = uv_s[i+1];
		const Vector2 & uv_c  = uv_s[i+2];
		uvs.set( 2*i,   uv_a );
		uvs.set( 2*i+1, uv_b );
		uvs.set( 2*i+2, uv_b );
		uvs.set( 2*i+3, uv_c );
		uvs.set( 2*i+4, uv_c );
		uvs.set( 2*i+5, uv_a );

		const Vector2 & uv2_a = uv2_s[i];
		const Vector2 & uv2_b = uv2_s[i+1];
		const Vector2 & uv2_c = uv2_s[i+2];
		uv2s.set( 2*i,   uv2_a );
		uv2s.set( 2*i+1, uv2_b );
		uv2s.set( 2*i+2, uv2_b );
		uv2s.set( 2*i+3, uv2_c );
		uv2s.set( 2*i+4, uv2_c );
		uv2s.set( 2*i+5, uv2_a );
	}

	Array arrays;
	arrays.resize( ArrayMesh::ARRAY_MAX );
	arrays.set( ArrayMesh::ARRAY_VERTEX,  vertices );
	arrays.set( ArrayMesh::ARRAY_NORMAL,  normals );
	arrays.set( ArrayMesh::ARRAY_TEX_UV,  uvs );
	arrays.set( ArrayMesh::ARRAY_TEX_UV2, uv2s );

	Ref<ArrayMesh> am = memnew(ArrayMesh);
	am->add_surface_from_arrays( Mesh::PRIMITIVE_LINES, arrays );

	mi->set_mesh( am );
}

Ref<Se3Ref> MarchingCubesDualGd::compute_source_se3( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & pt_in_source_se3, const Ref<DistanceScalerBaseRef> & scaler )
{
	Ref<Se3Ref> ret_se3;
	ret_se3.instance();

	const SE3 & source_se3        = src_se3->se3;
	const Vector3d & pt_in_source = pt_in_source_se3->se3.r_;

	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;


	ret_se3->se3 = cubes.compute_source_se3( source_se3, pt_in_source, s );
	return ret_se3;
}

Transform MarchingCubesDualGd::compute_source_transform( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & pt_in_source_se3, const Ref<DistanceScalerBaseRef> & scaler )
{
	const SE3 & source_se3        = src_se3->se3;
	const Vector3d & pt_in_source = pt_in_source_se3->se3.r_;

	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	const Transform transform = cubes.compute_source_transform( source_se3, pt_in_source, s );
	return transform;
}


const PoolVector3Array & MarchingCubesDualGd::collision_faces( const Ref<Se3Ref> & src_se3, real_t dist )
{
	const SE3 & source_se3 = src_se3->se3;

	const std::vector<Vector3> & faces = cubes.collision_faces( source_se3, dist );

	const int qty = faces.size();
	ret_pool_array.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const Vector3 & v = faces[i];
		ret_pool_array.set( i, v );
	}

	return ret_pool_array;
}

void MarchingCubesDualGd::set_max_nodes_qty( int qty )
{
	cubes.max_nodes_qty = qty;
}

int MarchingCubesDualGd::get_max_nodes_qty() const
{
	return cubes.max_nodes_qty;
}

int  MarchingCubesDualGd::get_nodes_qty() const
{
	const int ret = cubes.get_nodes_qty();
	return ret;
}

Array MarchingCubesDualGd::get_node( int node_ind ) const
{
	Array ret;
	ret.resize(8);
	Vector3d verts[8];

	cubes.get_node( node_ind, verts );
	for ( int i=0; i<8; i++ )
	{
		const Vector3d & v = verts[i];
		const Vector3 at( v.x_, v.y_, v.z_ );
		ret[i] = at;
	}
	return ret;
}

int  MarchingCubesDualGd::get_node_parent( int node_ind ) const
{
	const int ret = cubes.get_node_parent( node_ind );
	return ret;
}

int  MarchingCubesDualGd::get_dual_cells_qty() const
{
	const int qty = cubes.get_dual_cells_qty();
	return qty;
}

Array MarchingCubesDualGd::get_dual_cell( int cell_ind ) const
{
	Array ret;
	ret.resize(8);
	Vector3d verts[8];

	cubes.get_dual_cell( cell_ind, verts );
	for ( int i=0; i<8; i++ )
	{
		const Vector3d & v = verts[i];
		const Vector3 at( v.x_, v.y_, v.z_ );
		ret[i] = at;
	}
	return ret;
}






}




