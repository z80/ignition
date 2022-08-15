
#include "volume_mesh_gd.h"

namespace Ign
{

void VolumeMeshGd::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("set_source_se3", "se3"), &VolumeMeshGd::set_source_se3 );
	ClassDB::bind_method( D_METHOD("get_source_se3"),        &VolumeMeshGd::get_source_se3, Variant::OBJECT );

	ClassDB::bind_method( D_METHOD("materials"),                   &VolumeMeshGd::materials, Variant::ARRAY );
	ClassDB::bind_method( D_METHOD("precompute", "material_ind", "scaler"), &VolumeMeshGd::precompute );

	ClassDB::bind_method( D_METHOD("compute_source_se3", "scaler"), &VolumeMeshGd::compute_source_se3, Variant::OBJECT );
	ClassDB::bind_method( D_METHOD("compute_source_transform", "scaler"), &VolumeMeshGd::compute_source_transform, Variant::TRANSFORM );

	ClassDB::bind_method( D_METHOD("vertices"), &VolumeMeshGd::vertices, Variant::POOL_VECTOR3_ARRAY );
	ClassDB::bind_method( D_METHOD("normals"),  &VolumeMeshGd::normals,  Variant::POOL_VECTOR3_ARRAY );
	ClassDB::bind_method( D_METHOD("tangents"), &VolumeMeshGd::tangents, Variant::POOL_REAL_ARRAY );
	ClassDB::bind_method( D_METHOD("uvs"),      &VolumeMeshGd::uvs,      Variant::POOL_VECTOR2_ARRAY );
	ClassDB::bind_method( D_METHOD("uv2s"),     &VolumeMeshGd::uv2s,     Variant::POOL_VECTOR2_ARRAY );

	ADD_PROPERTY( PropertyInfo( Variant::OBJECT, "source_se3" ),   "set_source_transform", "get_source_transform" );

}

VolumeMeshGd::VolumeMeshGd()
{
}

VolumeMeshGd::~VolumeMeshGd()
{
}

void VolumeMeshGd::set_source_se3( const Ref<Se3Ref> & se3 )
{
	_mesh.set_source_se3( se3->se3 );
}

Ref<Se3Ref> VolumeMeshGd::get_source_se3() const
{
	Ref<Se3Ref> se3;
	se3.instance();

	se3->se3 = _mesh.get_source_se3();

	return se3;
}

const Array & VolumeMeshGd::materials()
{
	const std::vector<int> & mats = _mesh.materials();
	const int qty = mats.size();
	_materials.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const int ind = mats[i];
		_materials.set( i , ind );
	}

	return _materials;
}

void VolumeMeshGd::precompute( int material_ind, const Ref<DistanceScalerBaseRef> & scaler )
{
	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	_mesh.precompute( material_ind, s );
}

Ref<Se3Ref> VolumeMeshGd::compute_source_se3( const Ref<DistanceScalerBaseRef> & scaler ) const
{
	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	Ref<Se3Ref> se3;
	se3.instance();

	se3->se3 = _mesh.compute_source_se3( s );

	return se3;
}

Transform VolumeMeshGd::compute_source_transform( const Ref<DistanceScalerBaseRef> & scaler ) const
{
	const DistanceScalerBaseRef * scaler_ref = scaler.ptr();
	const DistanceScalerBase * s;
	if ( scaler_ref == nullptr )
		s = nullptr;
	else
		s = scaler_ref->scaler_base;

	const Transform t = _mesh.compute_source_transform( s );

	return t;
}

const PoolVector3Array & VolumeMeshGd::vertices()
{
	const std::vector<Vector3> & verts = _mesh.vertices();
	const int qty = verts.size();
	_vertices.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const Vector3 & v = verts[i];
		_vertices.set( i, v );
	}

	return _vertices;
}

const PoolVector3Array & VolumeMeshGd::normals()
{
	const std::vector<Vector3> & norms = _mesh.normals();
	const int qty = norms.size();
	_normals.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const Vector3 & v = norms[i];
		_normals.set( i, v );
	}

	return _normals;
}

const PoolRealArray    & VolumeMeshGd::tangents()
{
	const std::vector<real_t> & tangs = _mesh.tangents();
	const int qty = tangs.size();
	_tangents.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const real_t v = tangs[i];
		_tangents.set( i, v );
	}

	return _tangents;
}

const PoolVector2Array & VolumeMeshGd::uvs()
{
	const std::vector<Vector2> & uvs = _mesh.uvs();
	const int qty = uvs.size();
	_uvs.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const Vector2 & uv = uvs[i];
		_uvs.set( i, uv );
	}

	return _uvs;
}

const PoolVector2Array & VolumeMeshGd::uv2s()
{
	const std::vector<Vector2> & uv2s = _mesh.uv2s();
	const int qty = uv2s.size();
	_uv2s.resize( qty );
	for ( int i=0; i<qty; i++ )
	{
		const Vector2 & uv = uv2s[i];
		_uv2s.set( i, uv );
	}

	return _uv2s;
}



}






