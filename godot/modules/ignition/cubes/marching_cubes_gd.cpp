
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
	ClassDB::bind_method( D_METHOD("faces"),                                &MarchingCubesGd::faces,            Variant::POOL_VECTOR3_ARRAY );
	ClassDB::bind_method( D_METHOD("apply_to_mesh", "mesh_instance"),       &MarchingCubesGd::apply_to_mesh );
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

PoolVector3Array MarchingCubesGd::faces() const
{
	const Vector<Face3> & f = cubes.faces();
	PoolVector3Array ret;
	const int qty = f.size();
	const int pts_qty = qty * 3;
	ret.resize( pts_qty );

	int ind = 0;
	for ( int i=0; i<qty; i++ )
	{
		const Face3 & face = f.ptr()[i];
		ret.set( ind,   face.vertex[0] );
		ret.set( ind+1, face.vertex[1] );
		ret.set( ind+2, face.vertex[2] );
		ind += 3;
	}

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

	const Vector<Face3> & f = cubes.faces();
	const int tris_qty  = f.size();
	const int verts_qty = tris_qty * 3;
	// Fill in arrays.
	vertices.resize( verts_qty );
	normals.resize( verts_qty );
	tangents.resize( verts_qty*4 );
	//colors.resize( verts_qty );
	//uvs.resize( verts_qty );
	//uvs2.resize( qty );

	for ( int i=0; i<tris_qty; i++ )
	{
		const Face3 & face = f.ptr()[i];
		const Vector3 norm = face.get_plane().normal;
		const Vector3 tangent = norm.cross( face.vertex[2] - face.vertex[0] ).normalized();
		for ( int j=0; j<3; j++ )
		{
			const Vector3 & v = face.vertex[j];

			vertices.set( i, v );
			normals.set( i, norm );
			int ind = i*4;
			tangents.set( ind++, tangent.x );
			tangents.set( ind++, tangent.y );
			tangents.set( ind++, tangent.z );
			tangents.set( ind, 1.0 );
			//const Color c = v.color;
			//colors.set( i, c );
			//uvs.set( i, uv );
			//uvs2.set( i, uv2 );
		}
	}

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





}




