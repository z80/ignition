
#include "core/object/ref_counted.h"
#include "se3_ref.h"
#include "marching_cubes_dual_node.h"
#include "marching_cubes_dual_gd.h"



#ifndef __BOUNDING_NODE_H_
#define __BOUNDING_NODE_H_

namespace Ign
{

class MarchingCubesDualGd;

class BoundingNodeGd: public RefCounted
{
	GDCLASS(BoundingNodeGd, RefCounted);
protected:
	static void _bind_methods();

public:
	BoundingNodeGd();
	~BoundingNodeGd();

	Ref<BoundingNodeGd> create_adjacent_node( int dx, int dy, int dz ) const;
	bool equals_to( const Ref<BoundingNodeGd> & other ) const;
	String get_node_id() const;
	String get_adjacent_node_id( int dx, int dy, int dz ) const;

	Ref<Se3Ref> get_center( const Ref<MarchingCubesDualGd> & surface ) const;
	real_t      get_size( const Ref<MarchingCubesDualGd> & surface ) const;
	String      get_hash() const;


public:
	MarchingCubesDualNode node;
};


}


#endif

