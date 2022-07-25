
#ifndef __MARCHING_CUBES_DUAL_GD_H_
#define __MARCHING_CUBES_DUAL_GD_H_

#include "marching_cubes_dual.h"
#include "core/reference.h"
#include "se3_ref.h"
#include "volume_source_gd.h"
#include "distance_scaler_ref.h"
#include "marching_cubes_dual_node_gd.h"

class Node;

namespace Ign
{

class MarchingCubesDualGd: public Reference
{
	GDCLASS(MarchingCubesDualGd, Reference);
protected:
	static void _bind_methods();

public:
	MarchingCubesDualGd();
	~MarchingCubesDualGd();

	void set_source_transform( const Ref<Se3Ref> & se3 );

	void set_split_precision( real_t rel_diff );
	real_t get_split_precision() const;

	bool subdivide_source( real_t bounding_radius, const Ref<VolumeSourceGd> & volume, const Ref<DistanceScalerRef> & scaler = Ref<DistanceScalerRef>() );

	Array query_close_nodes( const Vector3 & at, real_t dist, real_t max_size );
	Vector3 center_direction( const Vector3 & at, bool in_source ) const;
	Ref<MarchingCubesDualNodeGd> get_tree_node( int ind );


	Array materials_used() const;
	void apply_to_mesh( int material_index, Node * mesh_instance, const Ref<DistanceScalerRef> & scaler = Ref<DistanceScalerRef>() );
	Array collision_faces( const Vector3 & at, real_t dist, bool in_source );

	void set_max_nodes_qty( int qty );
	int get_max_nodes_qty() const;

	// Debug methods.
	int  get_nodes_qty() const;
	Array get_node( int node_ind ) const;
	int  get_node_parent( int node_ind ) const;

	int  get_dual_cells_qty() const;
	Array get_dual_cell( int cell_ind ) const;


public:
	MarchingCubesDual cubes;

	PoolVector3Array vertices,
		             normals;
	PoolRealArray    tangents;
	Array            ret_array;
};




}





#endif







