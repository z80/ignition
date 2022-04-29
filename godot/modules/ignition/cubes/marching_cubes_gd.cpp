
#include "marching_cubes_gd.h"

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"


namespace Ign
{

void MarchingCubesGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_source_transform", "se3"),                              &MarchingCubesGd::set_source_transform );
	ClassDB::bind_method( D_METHOD("subdivide_source", "volume", "material", "scaler"),         &MarchingCubesGd::subdivide_source, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("materials_used"),                                           &MarchingCubesGd::materials_used,   Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("apply_to_mesh", "material_ind", "mesh_instance", "scaler"), &MarchingCubesGd::apply_to_mesh );

	ClassDB::bind_method( D_METHOD("set_max_nodes_qty", "qty"),             &MarchingCubesGd::set_max_nodes_qty );
	ClassDB::bind_method( D_METHOD("get_max_nodes_qty"),                    &MarchingCubesGd::get_max_nodes_qty, Variant::INT );

	ADD_PROPERTY( PropertyInfo( Variant::INT,   "max_nodes_qty" ),         "set_max_nodes_qty", "get_max_nodes_qty" );
}

MarchingCubesGd::MarchingCubesGd()
	: Reference()
{
}

MarchingCubesGd::~MarchingCubesGd()
{
}

void MarchingCubesGd::set_source_transform( const Ref<Se3Ref> & se3 )
{
	const SE3 & se3_inst = se3.ptr()->se3;
	cubes.set_source_transform( se3_inst );
}

bool MarchingCubesGd::subdivide_source( const Ref<VolumeSourceGd> & volume, const Ref<MaterialSourceGd> & material, const Ref<DistanceScalerRef> & scaler )
{
	VolumeSource   * volume_source   = volume.ptr()->source;
	MaterialSource * material_source = material.ptr()->source;
	const DistanceScalerRef * distance_scaler_ref = scaler.ptr();
	const DistanceScaler * distance_scaler = (distance_scaler_ref != nullptr) ? &(distance_scaler_ref->scaler) : nullptr;

	const bool ret = cubes.subdivide_source( volume_source, material_source, distance_scaler );
	return ret;
}

Array MarchingCubesGd::materials_used() const
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





void MarchingCubesGd::apply_to_mesh( int material_index, Node * mesh_instance, const Ref<DistanceScalerRef> & scaler )
{
	MeshInstance * mi = Object::cast_to<MeshInstance>(mesh_instance);
	if (mi == nullptr)
	{
		print_line( String( "ERROR: expects mesh instance as an argument, got something else." ) );
		return;
	}

	const std::vector<Vector3> & verts = cubes.vertices( material_index );
	const std::vector<Vector3> & norms = cubes.normals( material_index );
	const std::vector<real_t> & tangs = cubes.tangents( material_index );
	const int verts_qty = verts.size();
	// Fill in arrays.
	vertices.resize( verts_qty );
	normals.resize( verts_qty );
	tangents.resize( verts_qty*4 );
	//colors.resize( verts_qty );
	//uvs.resize( verts_qty );
	//uvs2.resize( qty );

	int vert_ind = 0;
	int tang_ind = 0;
	for ( int i=0; i<verts_qty; i++ )
	{
		const Vector3 vert = verts[i];
		const Vector3 norm = norms[i];
		vertices.set( i, vert );
		normals.set( i, norm );
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
	//arrays.set( ArrayMesh::ARRAY_TEX_UV,  uvs );
	//arrays.set( ArrayMesh::ARRAY_TEX_UV2, uvs2 );

	Ref<ArrayMesh> am = memnew(ArrayMesh);
	am->add_surface_from_arrays( Mesh::PRIMITIVE_TRIANGLES, arrays );

	mi->set_mesh( am );

	const DistanceScalerRef * scaler_ref = scaler.ptr();
	const DistanceScaler * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = &(scaler_ref->scaler);
	const Transform transform = cubes.source_transform( s );
	mi->set_transform( transform );
}

void MarchingCubesGd::set_max_nodes_qty( int qty )
{
	cubes.max_nodes_qty = qty;
}

int MarchingCubesGd::get_max_nodes_qty() const
{
	return cubes.max_nodes_qty;
}






}




