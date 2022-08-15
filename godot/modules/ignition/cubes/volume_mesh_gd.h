
#ifndef __VOLUME_MESH_GD_H_
#define __VOLUME_MESH_GD_H_

#include "volume_mesh.h"
#include "se3_ref.h"
#include "distance_scaler_base_ref.h"
#include "core/reference.h"

namespace Ign
{

class VolumeMeshGd: public Reference
{
	GDCLASS(VolumeMeshGd, Reference);
protected:
	static void _bind_methods();

public:
	VolumeMeshGd();
	~VolumeMeshGd();

	void set_source_se3( const Ref<Se3Ref> & se3 );
	Ref<Se3Ref> get_source_se3() const;

	const Array & materials();
	void precompute( int material_ind, const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() );

	Ref<Se3Ref> compute_source_se3( const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() ) const;
	Transform compute_source_transform( const Ref<DistanceScalerBaseRef> & scaler = Ref<DistanceScalerBaseRef>() ) const;

	const PoolVector3Array & vertices();
	const PoolVector3Array & normals();
	const PoolRealArray    & tangents();
	const PoolVector2Array & uvs();
	const PoolVector2Array & uv2s();

public:
	VolumeMesh _mesh;

	PoolVector3Array _vertices,
		             _normals;
	PoolRealArray    _tangents;
	PoolVector2Array _uvs,
		             _uv2s;

	Array            _materials;

};

}


#endif


