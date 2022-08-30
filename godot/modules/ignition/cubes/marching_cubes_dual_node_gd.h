
#ifndef __MARCHING_CUBES_DUAL_NODE_GD_H_
#define __MARCHING_CUBES_DUAL_NODE_GD_H_

#include "core/reference.h"
#include "se3_ref.h"
#include "distance_scaler_ref.h"

namespace Ign
{

class MarchingCubesDual;
class MarchingCubesDualNode;

class MarchingCubesDualNodeGd: public Reference
{
	GDCLASS(MarchingCubesDualNodeGd, Reference);
protected:
	static void _bind_methods();

public:
	MarchingCubesDualNodeGd();
	~MarchingCubesDualNodeGd();

	Array intersect_with_segment( const Vector3 & start, const Vector3 & end, const Ref<Se3Ref> & se3=Ref<Se3Ref>() );
	Array intersect_with_ray( const Vector3 & start, const Vector3 & dir, const Ref<Se3Ref> & se3=Ref<Se3Ref>() );
	String hierarchy_path() const;
	bool contains_point( const Vector3 & at, const Ref<Se3Ref> & se3=Ref<Se3Ref>() ) const;
	Vector3 center_vector( const Ref<Se3Ref> & se3=Ref<Se3Ref>() ) const;
	real_t node_size() const;
	Ref<Se3Ref> se3_in_point( const Vector3 & at, bool in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<Se3Ref> & inv_src_se3=Ref<Se3Ref>() ) const;
	Transform transform_in_point( const Vector3 & at, bool in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<Se3Ref> & inv_src_se3=Ref<Se3Ref>() ) const;
	String hash() const;

	Transform asset_transform( const Ref<Se3Ref> & asset_at, bool result_in_source, const Ref<Se3Ref> & src_se3=Ref<Se3Ref>(), const Ref<DistanceScalerRef> & scaler = Ref<DistanceScalerRef>() ) const;

	Array at();
	int   size() const;


public:
	MarchingCubesDual     * cubes;
	MarchingCubesDualNode * node;
	Array                   ret_array;
};



}






#endif



