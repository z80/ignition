
#ifndef __MARCHING_CUBES_DUAL_NODE_GD_H_
#define __MARCHING_CUBES_DUAL_NODE_GD_H_

#include "core/object/ref_counted.h"
#include "se3_ref.h"

namespace Ign
{

class MarchingCubesDual;
class MarchingCubesDualNode;

class MarchingCubesDualNodeGd: public RefCounted
{
	GDCLASS(MarchingCubesDualNodeGd, RefCounted);
protected:
	static void _bind_methods();

public:
	MarchingCubesDualNodeGd();
	~MarchingCubesDualNodeGd();

	Array intersect_with_segment( const Vector3 & start, const Vector3 & end );
	Array intersect_with_ray( const Vector3 & start, const Vector3 & dir );
	String hierarchy_path() const;
	bool contains_point( const Vector3 & at ) const;
	Vector3 center_vector() const;
	real_t node_size() const;
	Ref<Se3Ref> se3_in_point( const Vector3 & at ) const;
	Transform3D transform_in_point( const Vector3 & at ) const;
	String hash() const;

	Transform3D asset_transform( const Ref<Se3Ref> & src_se3, const Ref<Se3Ref> & asset_at ) const;

	Array at();
	int   size() const;


public:
	MarchingCubesDual     * cubes;
	MarchingCubesDualNode * node;
	Array                   ret_array;
};



}






#endif



