
#ifndef __VOLUME_MESH_H_
#define __VOLUME_MESH_H_

#include "data_types.h"
#include "cube_types.h"
#include "se3.h"
#include "vector3d.h"
#include "core/vector.h"

#include <vector>

namespace Ign
{

class DistanceScalerBase;

class VolumeMesh
{
public:
	VolumeMesh();
	~VolumeMesh();

	VolumeMesh( const VolumeMesh & inst );
	const VolumeMesh & operator=( const VolumeMesh & inst );

	void set_source_se3( const SE3 & se3 );
	const SE3 & get_source_se3() const;

	const std::vector<int> & materials() const;
	void precompute( int material_ind, const DistanceScalerBase * scaler );

	SE3 compute_source_se3( const DistanceScalerBase * scaler = nullptr ) const;
	Transform compute_source_transform( const DistanceScalerBase * scaler = nullptr ) const;

	const std::vector<Vector3> & vertices() const;
	const std::vector<Vector3> & normals() const;
	const std::vector<real_t>  & tangents() const;
	const std::vector<Vector2> & uvs() const;
	const std::vector<Vector2> & uv2s() const;

public:
	// Source transform.
	// Points to probe first are sent through this transformation.
	SE3      source_se3;
	SE3      inverted_source_se3;

	std::vector<NodeFace> _all_faces;
	std::vector<int>      _material_inds;

	std::vector<Vector3>  _ret_verts;
	std::vector<Vector3>  _ret_norms;
	std::vector<real_t>   _ret_tangs;
	std::vector<Vector2>  _ret_uvs, _ret_uv2s;
};


}






#endif

