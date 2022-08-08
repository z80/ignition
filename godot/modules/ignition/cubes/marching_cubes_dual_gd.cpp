
#include "marching_cubes_dual_gd.h"

#include "marching_cubes_dual_node.h"

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"


namespace Ign
{

void MarchingCubesDualGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_source_transform", "se3"),                              &MarchingCubesDualGd::set_source_transform );
	ClassDB::bind_method( D_METHOD("get_source_transform"),                                     &MarchingCubesDualGd::get_source_transform, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("set_split_precision", "rel_diff"),                          &MarchingCubesDualGd::set_split_precision );
	ClassDB::bind_method( D_METHOD("get_split_precision"),                                      &MarchingCubesDualGd::get_split_precision, Variant::REAL );

	ClassDB::bind_method( D_METHOD("subdivide_source", "radius", "volume", "scaler"),           &MarchingCubesDualGd::subdivide_source, Variant::BOOL );

	ClassDB::bind_method( D_METHOD("query_close_nodes", "at", "dist", "max_size"), &MarchingCubesDualGd::query_close_nodes, Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("center_direction", "at", "in_source"),   &MarchingCubesDualGd::center_direction,  Variant::VECTOR3 );
	ClassDB::bind_method( D_METHOD("get_tree_node", "ind"),                  &MarchingCubesDualGd::get_tree_node,     Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("materials_used"),                                           &MarchingCubesDualGd::materials_used,   Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("apply_to_mesh", "material_ind", "mesh_instance", "scaler"), &MarchingCubesDualGd::apply_to_mesh );
	ClassDB::bind_method( D_METHOD("mesh_transform", "scaler"),                                 &MarchingCubesDualGd::mesh_transform, Variant::TRANSFORM );
	ClassDB::bind_method( D_METHOD("collision_faces", "at", "dist", "in_source"),               &MarchingCubesDualGd::collision_faces, Variant::ARRAY );

	ClassDB::bind_method( D_METHOD("set_max_nodes_qty", "qty"),             &MarchingCubesDualGd::set_max_nodes_qty );
	ClassDB::bind_method( D_METHOD("get_max_nodes_qty"),                    &MarchingCubesDualGd::get_max_nodes_qty, Variant::INT );

	ClassDB::bind_method( D_METHOD("get_nodes_qty"),               &MarchingCubesDualGd::get_nodes_qty,   Variant::INT );
	ClassDB::bind_method( D_METHOD("get_node", "node_ind"),        &MarchingCubesDualGd::get_node,        Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("get_node_parent", "node_ind"), &MarchingCubesDualGd::get_node_parent, Variant::INT );

	ClassDB::bind_method( D_METHOD("get_dual_cells_qty"),          &MarchingCubesDualGd::get_dual_cells_qty, Variant::INT );
	ClassDB::bind_method( D_METHOD("get_dual_cell", "cell_ind"),   &MarchingCubesDualGd::get_dual_cell,      Variant::ARRAY );


	ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "source_se3" ),   "set_source_transform", "get_source_transform" );
	ADD_PROPERTY( PropertyInfo( Variant::INT, "split_precision" ), "set_split_precision",  "get_split_precision" );
	ADD_PROPERTY( PropertyInfo( Variant::INT, "max_nodes_qty" ),   "set_max_nodes_qty",    "get_max_nodes_qty" );
}

MarchingCubesDualGd::MarchingCubesDualGd()
	: Reference()
{
}

MarchingCubesDualGd::~MarchingCubesDualGd()
{
}

void MarchingCubesDualGd::set_source_transform( const Ref<Se3Ref> & se3 )
{
	const SE3 & se3_inst = se3.ptr()->se3;
	cubes.set_source_transform( se3_inst );
}

Ref<Se3Ref> MarchingCubesDualGd::get_source_transform() const
{
	Ref<Se3Ref> se3;
	se3.instance();

	se3->se3 = cubes.get_source_transform();

	return se3;
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


bool MarchingCubesDualGd::subdivide_source( real_t bounding_radius, const Ref<VolumeSourceGd> & volume, const Ref<DistanceScalerRef> & scaler )
{
	VolumeSource   * volume_source   = volume.ptr()->source;
	const DistanceScalerRef * distance_scaler_ref = scaler.ptr();
	const DistanceScaler * distance_scaler = (distance_scaler_ref != nullptr) ? &(distance_scaler_ref->scaler) : nullptr;

	const bool ret = cubes.subdivide_source( bounding_radius, volume_source, distance_scaler );
	return ret;
}

Array MarchingCubesDualGd::query_close_nodes( const Vector3 & at, real_t dist, real_t max_size )
{
	ret_array.clear();
	const Vector3d at_d = Vector3d( at.x, at.y, at.z );
	const std::vector<int> indices = cubes.query_close_nodes( at, dist, max_size );
	const int qty = indices.size();
	for ( int i=0; i<qty; i++ )
	{
		const int ind = indices[i];
		ret_array.push_back( ind );
	}

	return ret_array;
}

Vector3 MarchingCubesDualGd::center_direction( const Vector3 & at, bool in_source ) const
{
	const Vector3d c = cubes.center_direction( Vector3d( at.x, at.y, at.z ), in_source );
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


Array MarchingCubesDualGd::materials_used() const
{
	Array ret;
	const std::set<int> mats = cubes.materials();
	int qty = 0;
	for ( std::set<int>::const_iterator it=mats.begin(); it!=mats.end(); it++ )
	{
		qty += 1;
	}
	ret.resize( qty );
	int ind = 0;
	for ( std::set<int>::const_iterator it=mats.begin(); it!=mats.end(); it++ )
	{
		const int mat = *it;
		ret.set( ind, mat );
		ind += 1;
	}
	return ret;
}





void MarchingCubesDualGd::apply_to_mesh( int material_index, Node * mesh_instance, const Ref<DistanceScalerRef> & scaler )
{
	MeshInstance * mi = Object::cast_to<MeshInstance>(mesh_instance);
	if (mi == nullptr)
	{
		print_line( String( "ERROR: expects mesh instance as an argument, got something else." ) );
		return;
	}

	const DistanceScalerRef * scaler_ref = scaler.ptr();
	const DistanceScaler * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = &(scaler_ref->scaler);

	const std::vector<Vector3> & verts = cubes.vertices( material_index, s );
	const std::vector<Vector3> & norms = cubes.normals( material_index );
	const std::vector<real_t> & tangs = cubes.tangents( material_index );
	const std::vector<Vector2> * p_uvs;
	const std::vector<Vector2> * p_uv2s;
	cubes.uvs( material_index, p_uvs, p_uv2s );

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

Transform MarchingCubesDualGd::mesh_transform( const Ref<DistanceScalerRef> & scaler )
{
	const DistanceScalerRef * scaler_ref = scaler.ptr();
	const DistanceScaler * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = &(scaler_ref->scaler);
	const Transform transform = cubes.compute_source_transform( s );
	return transform;
}


const PoolVector3Array & MarchingCubesDualGd::collision_faces( const Vector3 & at, real_t dist, bool in_source )
{
	const Vector3d at_d = Vector3d( at.x, at.y, at.z );
	const std::vector<Vector3> & faces = cubes.collision_faces( at_d, dist, in_source );
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




