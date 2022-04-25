
#include "marching_cubes_gd.h"

#include "scene/3d/mesh_instance.h"
#include "scene/resources/mesh.h"
#include "core/reference.h"


namespace Ign
{

void MarchingCubesGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_source_transform", "se3"),          &MarchingCubesGd::set_source_transform );
	ClassDB::bind_method( D_METHOD("subdivide_source", "source", "scaler"), &MarchingCubesGd::subdivide_source, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("apply_to_mesh", "mesh_instance"),       &MarchingCubesGd::apply_to_mesh );

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

bool MarchingCubesGd::subdivide_source( const Ref<VolumeSourceGd> & source, const Ref<DistanceScalerRef> & scaler )
{
	VolumeSource * volume_source = source.ptr()->source;
	const DistanceScalerRef * distance_scaler_ref = scaler.ptr();
	const DistanceScaler * distance_scaler = (distance_scaler_ref != nullptr) ? &(distance_scaler_ref->scaler) : nullptr;

	const bool ret = cubes.subdivide_source( volume_source, distance_scaler );
	return ret;
}





void MarchingCubesGd::apply_to_mesh( Node * mesh_instance )
{
	MeshInstance * mi = Object::cast_to<MeshInstance>(mesh_instance);
	if (mi == nullptr)
	{
		print_line( String( "ERROR: expects mesh instance as an argument, got something else." ) );
		return;
	}

	const Vector<Vector3> & verts = cubes.vertices();
	const Vector<Vector3> & norms = cubes.normals();
	const Vector<real_t> & tangs = cubes.tangents();
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
		const Vector3 vert = verts.ptr()[i];
		const Vector3 norm = norms.ptr()[i];
		vertices.set( i, vert );
		normals.set( i, norm );
	}

	const int tangs_qty = tangs.size();
	for ( int i=0; i<tangs_qty; i++ )
	{
		const real_t tang = tangs.ptr()[i];
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




