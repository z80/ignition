
#ifndef __MARCHING_CUBES_DUAL_NODE_GD_H_
#define __MARCHING_CUBES_DUAL_NODE_GD_H_

#include "core/reference.h"
#include "se3_ref.h"

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

	Array intersect_with_segment( const Vector3 & start, const Vector3 & end, bool in_source );
	Array intersect_with_ray( const Vector3 & start, const Vector3 & dir, bool in_source );
	bool contains_point( const Vector3 & at ) const;
	Vector3 center_vector( bool in_source ) const;
	Ref<Se3Ref> se3_in_point( const Vector3 & at, bool in_source ) const;
	Transform transform_in_point( const Vector3 & at, bool in_source ) const;
	String hash() const;


public:
	MarchingCubesDual     * cubes;
	MarchingCubesDualNode * node;
	Array                   ret_array;
};



}






#endif



