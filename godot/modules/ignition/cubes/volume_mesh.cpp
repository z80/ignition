
#include "volume_mesh.h"
#include "distance_scaler_base.h"

namespace Ign
{

VolumeMesh::VolumeMesh()
{
}

VolumeMesh::~VolumeMesh()
{
}

VolumeMesh::VolumeMesh( const VolumeMesh & inst )
{
	*this = inst;
}

const VolumeMesh & VolumeMesh::operator=( const VolumeMesh & inst )
{
	if ( this != &inst )
	{
		source_se3          = inst.source_se3;
		inverted_source_se3 = inst.inverted_source_se3;

		_all_faces          = inst._all_faces;
		_material_inds      = inst._material_inds;
	}
	return *this;
}

void VolumeMesh::set_source_se3( const SE3 & se3 )
{
	source_se3          = se3;
	inverted_source_se3 = se3.inverse();
}

const SE3 & VolumeMesh::get_source_se3() const
{
	return source_se3;
}

const std::vector<int> & VolumeMesh::materials() const
{
	return _material_inds;
}

void VolumeMesh::precompute( int material_ind, const DistanceScalerBase * scaler )
{
	const unsigned int qty = _all_faces.size();
	_ret_verts.clear();
	_ret_verts.reserve(3*qty);

	_ret_norms.clear();
	_ret_norms.reserve(3*qty);

	_ret_tangs.clear();
	_ret_verts.reserve(4*qty);

	_ret_uvs.clear();
	_ret_uvs.reserve(3*qty);

	_ret_uv2s.clear();
	_ret_uv2s.reserve(3*qty);

	const SE3 scaled_source_se3 = compute_source_se3( scaler );
	const SE3 inv_scaled_source_se3 = scaled_source_se3.inverse();

	for ( unsigned int i=0; i<qty; i++ )
	{
		const NodeFace & f = _all_faces[i];
		const int face_material_ind = f.material_ind;
		if ( face_material_ind != material_ind )
			continue;
		for ( int j=0; j<3; j++ )
		{
			// Vertices.
			const Vector3d & source_v = f.vertices[j];
			// Convert to world.
			Vector3d world_v = source_se3 * source_v;
			// Convert to scaled world.
			if ( scaler != nullptr )
				world_v = scaler->scale( world_v );
			// Convert relative to scaled origin.
			const Vector3d rel_to_scaled_v = inv_scaled_source_se3 * world_v;

			_ret_verts.push_back( Vector3( rel_to_scaled_v.x_, rel_to_scaled_v.y_, rel_to_scaled_v.z_ ) );

			// Normals.
			const Vector3d & norm = f.normals[j];
			_ret_norms.push_back( Vector3( norm.x_, norm.y_, norm.z_ ) );

			// UVs.
			_ret_uvs.push_back( Vector2( source_v.x_, source_v.y_ ) );
			_ret_uv2s.push_back( Vector2( source_v.y_, source_v.z_ ) );
		}

		// Tangents.
		const Vector3d & t = f.tangent;
		_ret_tangs.push_back( t.x_ );
		_ret_tangs.push_back( t.y_ );
		_ret_tangs.push_back( t.z_ );
		_ret_tangs.push_back( 1.0 );
	}
}

SE3 VolumeMesh::compute_source_se3( const DistanceScalerBase * scaler ) const
{
	Vector3d o;
	if (scaler == nullptr)
		o = source_se3.r_;
	else
		o = scaler->scale( source_se3.r_ );

	SE3 se3( source_se3 );
	se3.r_ = o;

	return se3;
}

Transform VolumeMesh::compute_source_transform( const DistanceScalerBase * scaler ) const
{
	const SE3 se3 = compute_source_se3( scaler );
	const Transform ret = se3.transform();
	return ret;
}

const std::vector<Vector3> & VolumeMesh::vertices() const
{
	return _ret_verts;
}

const std::vector<Vector3> & VolumeMesh::normals() const
{
	return _ret_norms;
}

const std::vector<real_t>  & VolumeMesh::tangents() const
{
	return _ret_tangs;
}

const std::vector<Vector2> & VolumeMesh::uvs() const
{
	return _ret_uvs;
}

const std::vector<Vector2> & VolumeMesh::uv2s() const
{
	return _ret_uv2s;
}

std::vector<NodeFace> & VolumeMesh::node_faces()
{
	return _all_faces;
}

std::vector<int> & VolumeMesh::materials()
{
	return _material_inds;
}




}


